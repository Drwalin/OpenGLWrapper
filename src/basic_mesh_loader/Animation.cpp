/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2023 Marek Zalewski aka Drwalin
 *
 *  OpenGLWrapper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenGLWrapper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdio>

#include <algorithm>

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vector_relational.hpp>
#include <glm/mat4x4.hpp>

#include <assimp/scene.h>
#include <assimp/quaternion.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

#include "../../include/openglwrapper/basic_mesh_loader/Animation.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/AssimpLoader.hpp"

namespace gl {
namespace BasicMeshLoader {
	void Animation::LoadAnimation(class AssimpLoader* loader, const aiAnimation* anim, std::shared_ptr<Mesh> mesh) {
		this->scene = loader->scene;
		this->skeleton = mesh->skeleton;
		this->aiAnim = anim;
		duration = anim->mDuration;
		framesPerSecond = 24.0f;
		name = anim->mName.C_Str();
		duration = anim->mDuration/framesPerSecond;
	}
	
	void Animation::GetModelBoneMatrices(
			std::vector<glm::mat4>& matrices,
			float time, bool loop) {
		if(loop) {
			time = fmod(time, duration);
		}
		time *= framesPerSecond;
		matrices.resize(skeleton->bones.size());
		
		ReadNodeHierarchy(matrices, time, scene->mRootNode, glm::mat4(1));
	}
	
// 	aiNode* Animation::FindRootNode(
// 			aiNode* node,
// 			glm::mat4& transform) {
// 		if(skeleton->boneNameToId.find(node->mName.C_Str()) != skeleton->boneNameToId.end()) {
// 			return node;
// 		}
// 		transform *= ConvertAssimpToGlmMat(node->mTransformation);
// 		for(int i=0; i<node->mNumChildren; ++i) {
// 			glm::mat4 t = transform;
// 			aiNode* n = FindRootNode(node->mChildren[i], t);
// 			if(n) {
// 				transform = t;
// 				return n;
// 			}
// 		}
// 		return NULL;
// 	}
	
	static const aiNodeAnim* FindNodeAnim(const aiAnimation* anim, std::string name) {
		for(int i=0; i<anim->mNumChannels; ++i) {
			if(name == anim->mChannels[i]->mNodeName.C_Str()) {
				return anim->mChannels[i];
			}
		}
		return NULL;
	}
	
	
	
	static int FindRotation(float animationTime, const aiNodeAnim* nodeAnim) {
		for(uint i=0; i<nodeAnim->mNumRotationKeys-1; i++)
			if(animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		return -5;
	}
	
	static glm::mat4 CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim) {
		aiQuaternion out;
		if(nodeAnim->mNumRotationKeys == 1) {
			out = nodeAnim->mRotationKeys[0].mValue;
		} else if(nodeAnim->mNumRotationKeys == 0) {
			return glm::mat4(1);
		} else {
			uint index = FindRotation(animationTime, nodeAnim);
			if(index < 0)
				index = nodeAnim->mNumRotationKeys;
			uint nextIndex = (index + 1);
			if(nextIndex >= nodeAnim->mNumRotationKeys) {
				out = nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys-1].mValue;
			} else {
				float deltaTime = nodeAnim->mRotationKeys[nextIndex].mTime - nodeAnim->mRotationKeys[index].mTime;
				float factor = (animationTime - (float)nodeAnim->mRotationKeys[index].mTime) / deltaTime;
				if(factor < 0.0)
					factor = 0;
				else if(factor > 1)
					factor = 1;
				const aiQuaternion& start = nodeAnim->mRotationKeys[index].mValue;
				const aiQuaternion& end = nodeAnim->mRotationKeys[nextIndex].mValue;
				aiQuaternion::Interpolate(out, start, end, factor);
				out = out.Normalize();
			}
		}
		return glm::mat4_cast(glm::quat(out.w, out.x, out.y, out.z));
	}
	
	
	static int FindVector(float AnimationTime, int numKeys, const aiVectorKey* vectorKeys) {
		for(int i=0; i<numKeys-1; i++)
			if(AnimationTime < (float)vectorKeys[i + 1].mTime)
				return i;
		return -5;
	}
	
	static glm::vec3 CalcInterpolatedVector(glm::vec3 Default, float AnimationTime, int numKeys, const aiVectorKey* vectorKeys) {
		aiVector3D out;
		if(numKeys == 1) {
			out = vectorKeys[0].mValue;
		} else if(numKeys == 0) {
			return Default;
		} else {
			uint index = FindVector(AnimationTime, numKeys, vectorKeys);
			if(index < 0)
				index = numKeys;
			uint nextIndex = (index + 1);
			if(nextIndex >= numKeys) {
				out = vectorKeys[numKeys-1].mValue;
			} else {
				float deltaTime = vectorKeys[nextIndex].mTime - vectorKeys[index].mTime;
				float factor = (AnimationTime - (float)vectorKeys[index].mTime) / deltaTime;
				if(factor < 0.0)
					factor = 0;
				else if(factor > 1)
					factor = 1;
				auto start = vectorKeys[index].mValue;
				auto end = vectorKeys[nextIndex].mValue;
				out = start + (end-start)*factor;
			}
		}
		return glm::vec3(out.x, out.y, out.z); 
	}
	
	
	
	void Animation::ReadNodeHierarchy(std::vector<glm::mat4>& matrices,
			float time, const aiNode* pNode, 
			glm::mat4 parentTransform) {
		std::string nodeName = pNode->mName.C_Str();
		glm::mat4 nodeTransformation = ConvertAssimpToGlmMat(pNode->mTransformation);
		const aiNodeAnim* nodeAnim = FindNodeAnim(aiAnim, nodeName);
		int boneIndex = skeleton->GetBoneIndex(nodeName);
		
		// Interpolate animation for bone
		if(nodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			glm::mat4 scale = glm::scale(
					glm::mat4(1),
					CalcInterpolatedVector(
						glm::vec3(1),
						time,
						nodeAnim->mNumScalingKeys,
						nodeAnim->mScalingKeys));
			
			// Interpolate rotation and generate rotation transformation matrix
			glm::mat4 rotation = CalcInterpolatedRotation(time, nodeAnim);

			// Interpolate translation and generate translation transformation matrix
			glm::mat4 translate = glm::translate(
					glm::mat4(1),
					CalcInterpolatedVector(
						glm::vec3(0),
						time,
						nodeAnim->mNumPositionKeys,
						nodeAnim->mPositionKeys));

			// Combine the above transformations
			nodeTransformation = translate * rotation * scale;
		}
		
		// We only care for nodes that are bones in our animation and chosen skeleton
		if(boneIndex < 0) {
			nodeTransformation = glm::mat4(1);
		}
			
		glm::mat4 globalTransformation = parentTransform * nodeTransformation;
		
		if(boneIndex >= 0) {
			// Construct final matrix transformation for bone in position (used directly in animation shader)
			matrices[boneIndex] =
				globalTransformation
				* skeleton->bones[boneIndex].globalInverseBindingPoseMatrix;
		}

		// Traverse child nodes
		for(uint i=0; i<pNode->mNumChildren; ++i) {
			ReadNodeHierarchy(matrices, time, pNode->mChildren[i], globalTransformation);
		}
	}
} // namespace BasicMeshLoader
} // namespace gl


