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

#include "../../include/openglwrapper/basic_mesh_loader/AssimpLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

namespace gl {
namespace BasicMeshLoader {
	void AssimpLoader::Load(const char* file) {
		importer = std::make_shared<Assimp::Importer>();
		const ::aiScene* s = importer->ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		scene = s;
		
		meshNameToId.clear();
		meshes.clear();
		meshes.resize(s->mNumMeshes);
		for(int i=0; i<s->mNumMeshes; ++i) {
			meshes[i] = std::make_shared<Mesh>();
			meshes[i]->LoadMesh(s, s->mMeshes[i]);
			meshNameToId.emplace(meshes[i]->name, i);
		}
		
		/*
		skeletonNameToId.clear();
		skeletons.clear();
		skeletons.resize(s->mNumSkeletons);
		for(int i=0; i<s->mNumSkeletons; ++i) {
			skeletons[i] = std::make_shared<Skeleton>();
			skeletons[i]->LoadSkeleton(s->mSkeletons[i]);
			skeletonNameToId.emplace(skeletons[i]->name, i);
		}
		*/
		
		animationNameToId.clear();
		animations.clear();
		animations.resize(s->mNumAnimations);
		for(int i=0; i<s->mNumAnimations; ++i) {
			animations[i] = std::make_shared<Animation>();
			animations[i]->LoadAnimation(this, s->mAnimations[i], meshes[0]);
			animationNameToId.emplace(animations[i]->name, i);
		}
	}
	
	void AssimpLoader::GetModelBoneMatrices(
			std::shared_ptr<Animation> animation,
			std::shared_ptr<Mesh> mesh,
			std::vector<glm::mat4>& matrices,
			float time, bool loop) {
		if(loop) {
			time = fmod(time, animation->duration);
		}
		time *= animation->framesPerSecond;
		matrices.resize(mesh->bones.size());
		
// 		glm::mat4 transform(1.0f);
// 		aiNode* animationRootNode = FindRootNode(scene->mRootNode, transform, animation, mesh);
		
// 		ReadNodeHierarchy(matrices, animation->aiAnim, mesh, time, scene->mRootNode, glm::inverse(transform));//glm::mat4(1), false);
//		ReadNodeHierarchy(matrices, animation->aiAnim, mesh, time, animationRootNode, transform);//glm::mat4(1), false);
		ReadNodeHierarchy(matrices, animation->aiAnim, mesh, time, scene->mRootNode, glm::mat4(1), false);
		printf("\n\n\n\n\n\n");
	}
	
	aiNode* AssimpLoader::FindRootNode(
			aiNode* node,
			glm::mat4& transform,
			std::shared_ptr<Animation> anim,
			std::shared_ptr<Mesh> mesh) {
		if(mesh->boneNameToId.find(node->mName.C_Str()) != mesh->boneNameToId.end()) {
			return node;
		}
		transform *= ConvertAssimpToGlmMat(node->mTransformation);
		for(int i=0; i<node->mNumChildren; ++i) {
			glm::mat4 t = transform;
			aiNode* n = FindRootNode(node->mChildren[i], t, anim, mesh);
			if(n) {
				transform = t;
				return n;
			}
		}
		return NULL;
	}
	
	static const aiNodeAnim* FindNodeAnim(const aiAnimation* anim, std::string name) {
		for(int i=0; i<anim->mNumChannels; ++i) {
			if(name == anim->mChannels[i]->mNodeName.C_Str()) {
				return anim->mChannels[i];
			}
		}
		return NULL;
	}
	
	
	
	static int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
		for(uint i=0; i<pNodeAnim->mNumRotationKeys-1; i++)
			if(AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		return -5;
	}
	static glm::mat4 CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
		aiQuaternion Out;
		if(pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
		} else if(pNodeAnim->mNumRotationKeys == 0) {
			return glm::mat4(1);
		} else {
			uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
			if(RotationIndex < 0)
				RotationIndex = pNodeAnim->mNumRotationKeys;
			uint NextRotationIndex = (RotationIndex + 1);
			if(NextRotationIndex >= pNodeAnim->mNumRotationKeys) {
				Out = pNodeAnim->mRotationKeys[pNodeAnim->mNumRotationKeys-1].mValue;
			} else {
				float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
				float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
				if(Factor < 0.0)
					Factor = 0;
				else if(Factor > 1)
					Factor = 1;
				const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
				const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
				aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
				
				glm::quat a, b;
				a = {
					StartRotationQ.w,
					StartRotationQ.x,
					StartRotationQ.y,
					StartRotationQ.z
				};
				b = {
					EndRotationQ.w,
					EndRotationQ.x,
					EndRotationQ.y,
					EndRotationQ.z
				};
				
				Out = Out.Normalize();
			}
		}
		return glm::mat4_cast(glm::quat(Out.w, Out.x, Out.y, Out.z));
// 		return glm::mat4_cast(glm::quat(Out.x, Out.y, Out.z, Out.w));
	}
	
	
	static int FindVector(float AnimationTime, int numKeys, const aiVectorKey* vectorKeys) {
		for(int i=0; i<numKeys-1; i++)
			if(AnimationTime < (float)vectorKeys[i + 1].mTime)
				return i;
		return -5;
	}
	static glm::vec3 CalcInterpolatedVector(glm::vec3 Default, float AnimationTime, int numKeys, const aiVectorKey* vectorKeys) {
		aiVector3D Out;
		if(numKeys == 1) {
			Out = vectorKeys[0].mValue;
		} else if(numKeys == 0) {
			return Default;
		} else {
			uint RotationIndex = FindVector(AnimationTime, numKeys, vectorKeys);
			if(RotationIndex < 0)
				RotationIndex = numKeys;
			uint NextRotationIndex = (RotationIndex + 1);
			if(NextRotationIndex >= numKeys) {
				Out = vectorKeys[numKeys-1].mValue;
			} else {
				float DeltaTime = vectorKeys[NextRotationIndex].mTime - vectorKeys[RotationIndex].mTime;
				float Factor = (AnimationTime - (float)vectorKeys[RotationIndex].mTime) / DeltaTime;
				if(Factor < 0.0)
					Factor = 0;
				else if(Factor > 1)
					Factor = 1;
				auto start = vectorKeys[RotationIndex].mValue;
				auto end = vectorKeys[NextRotationIndex].mValue;
				Out = start + (end-start)*Factor;
			}
		}
		return glm::vec3(Out.x, Out.y, Out.z); 
	}
	
	
	
	void AssimpLoader::ReadNodeHierarchy(std::vector<glm::mat4>& matrices,
			const aiAnimation* animation,
			std::shared_ptr<Mesh> mesh,
			float time, const aiNode* pNode, 
			glm::mat4 parentTransform,
			bool isSkeleton) {
		std::string nodeName = pNode->mName.C_Str();
		glm::mat4 nodeTransformation = ConvertAssimpToGlmMat(pNode->mTransformation);
		const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);
		int boneIndex = mesh->GetBoneIndex(nodeName);
		
		if(nodeAnim) {
			isSkeleton = true;
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
		
		if(boneIndex < 0) {
			nodeTransformation = glm::mat4(1);
		}
			
			if(boneIndex >= 0) {
				printf(" node relative transfrmation animation '%s'\n", nodeName.c_str());
				printMat(nodeTransformation);
			}

		glm::mat4 globalTransformation = parentTransform * nodeTransformation;
		
		if(boneIndex >= 0) {
// 			printf(" global transform '%s'\n", nodeName.c_str());
// 			printMat(globalTransformation);

			matrices[boneIndex] =
// 				mesh->inverseGlobalMatrix *
// 				mesh->rootInverseMatrix *
				globalTransformation
				*
				mesh->bones[boneIndex].globalInverseBindingPoseMatrix
				;
		}

		for(uint i=0; i<pNode->mNumChildren; i++) {
			ReadNodeHierarchy(matrices, animation, mesh, time, pNode->mChildren[i], globalTransformation, isSkeleton);
		}
	}
	
		
	aiNode* AssimpLoader::FindNodeAndTransform(aiNode* node,
			const std::string& name,
			glm::mat4& parentTransform) {
		if(name == node->mName.C_Str()) {
			return node;
		}
		parentTransform *= ConvertAssimpToGlmMat(node->mTransformation);
		for(int i=0; i<node->mNumChildren; ++i) {
			glm::mat4 t = parentTransform;
			aiNode* n = FindNodeAndTransform(node->mChildren[i], name, t);
			if(n) {
				parentTransform = t;
				return n;
			}
		}
		return NULL;
	}
} // namespace BasicMeshLoader
} // namespace gl

