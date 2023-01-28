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
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vector_relational.hpp>
#include <openglwrapper/basic_mesh_loader/Animation.hpp>

#include <assimp/scene.h>
#include <assimp/quaternion.h>

namespace gl {
namespace BasicMeshLoader {
	void Animation::LoadAnimation(class AssimpLoader* loader, const aiAnimation* anim, std::shared_ptr<Mesh> mesh) {
		this->mesh = mesh;
		duration = anim->mDuration;
		framesPerSecond = 24.0f;
		name = anim->mName.C_Str();
		duration = anim->mDuration/framesPerSecond;
		
		keyFramesPerBone.clear();
		keyFramesPerBone.resize(mesh->bones.size());
		
		for(int c=0; c<anim->mNumChannels; ++c) {
			aiNodeAnim* ch = anim->mChannels[c];
			if(mesh->boneNameToId.find(ch->mNodeName.C_Str()) != mesh->boneNameToId.end()) {
				int boneId = mesh->boneNameToId[ch->mNodeName.C_Str()];
			
			printf(" channel %i -> %s\n", c, ch->mNodeName.C_Str());
			
			for(int i=0; i<ch->mNumPositionKeys; ++i) {
				auto K = ch->mPositionKeys[i];
				auto v = K.mValue;
				keyFramesPerBone[c].keyPos.emplace_back(VectorKey{K.mTime, {v.x,v.y,v.z}});
			}
			
			for(int i=0; i<ch->mNumScalingKeys; ++i) {
				auto K = ch->mScalingKeys[i];
				auto v = K.mValue;
				keyFramesPerBone[c].keyScale.emplace_back(VectorKey{K.mTime, {v.x,v.y,v.z}});
			}
			
			for(int i=0; i<ch->mNumRotationKeys; ++i) {
				auto K = ch->mRotationKeys[i];
				auto v = K.mValue;
				keyFramesPerBone[c].keyRot.emplace_back(QuatKey{K.mTime, {v.w,v.x,v.y,v.z}});
			}
			
			std::sort(
					keyFramesPerBone[c].keyPos.begin(),
					keyFramesPerBone[c].keyPos.end(), 
					[](VectorKey a, VectorKey b)->bool{
						return a.time < b.time;
					});
			
			std::sort(
					keyFramesPerBone[c].keyScale.begin(),
					keyFramesPerBone[c].keyScale.end(), 
					[](VectorKey a, VectorKey b)->bool{
						return a.time < b.time;
					});
			
			std::sort(
					keyFramesPerBone[c].keyRot.begin(),
					keyFramesPerBone[c].keyRot.end(), 
					[](QuatKey a, QuatKey b)->bool{
						return a.time < b.time;
					});
			}
		}
		
		printf(" animation name: %s\n", anim->mName.C_Str());
	}
	
	template<typename T>
	inline float BoneAnimation::Find(std::vector<T>& keys, float time, T& a, T& b) { 
		int i=0;
		for(i=0; i<keys.size(); ++i) {
			if(keys[i].time > time) {
				i = i-1;
				break;
			}
		}
		if(keys.size() == 0) {
			a = b = T();
			return 0;
		}
		if(i < 0) {
			a = b = keys[0];
			return 0;
		} else if(i >= keys.size()) {
			a = b = keys[keys.size()-1];
			return 0;
		}
		a = keys[i];
		b = keys[i+1];
		if(std::fabs(a.time-b.time) <= 0.000001) {
			return 0;
		}
		return (time-a.time)/(b.time-a.time);
	}
	
	glm::mat4 BoneAnimation::GetLocalMatrix(float time) {
		VectorKey ap, bp, as, bs;
		QuatKey ar, br;
		glm::vec3 p, s;
		glm::quat r;
		float tp, ts, tr;
		tp = Find(keyPos, time, ap, bp);
		ts = Find(keyScale, time, as, bs);
		tr = Find(keyRot, time, ar, br);
		p = ap.value + (bp.value-ap.value)*tp;
		s = as.value + (bs.value-as.value)*ts;
		r = glm::slerp(ar.value, br.value, tr);
		
		glm::mat4 trans = glm::translate(glm::mat4(1), p);
		glm::mat4 rot = glm::mat4_cast(r);
		glm::mat4 scale = glm::scale(glm::mat4(1), s);
		
		return trans*rot*scale;
	}
	
	void Animation::GetModelBoneMatrices(std::vector<glm::mat4>& matrices, float time, bool loop) {
		if(loop) {
			time = fmod(time, duration);
		}
		matrices.resize(keyFramesPerBone.size());
		for(int i=0; i<keyFramesPerBone.size(); ++i) {
			matrices[i] = keyFramesPerBone[i].GetLocalMatrix(time);
			int parent = mesh->bones[i].parentId;
			if(parent >= i) {
				throw "Error in: '" __FILE__ "' - bones need to be ordered (parent bone id must be lower than child, what is not the case here).";
			}
			//translate matrix from local to model, multiplying by parent
			if(parent >= 0) {
				matrices[i] = matrices[parent] * matrices[i];
			}
		}
		// encapsulate transformation from offset of a bone
		for(int i=0; i<keyFramesPerBone.size(); ++i) {
// 			matrices[i] = matrices[i] * this->mesh->bones[i].inverseLocalModelSpaceBindingPoseMatrix;
			matrices[i] = this->mesh->bones[i].inverseLocalModelSpaceBindingPoseMatrix * matrices[i];
		}
	}
} // namespace BasicMeshLoader
} // namespace gl


