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

#include "../../include/openglwrapper/basic_mesh_loader/Skeleton.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/Mesh.hpp"

#include <assimp/scene.h>

namespace gl {
namespace BasicMeshLoader {
	int Skeleton::GetBoneIndex(std::string boneName) {
		auto it = boneNameToId.find(boneName);
		if(it == boneNameToId.end())
			return -1;
		return it->second;
	}
	
	void Skeleton::LoadSkeleton(Mesh* cmesh, const aiMesh* mesh, const aiScene* scene) {
		if(mesh->HasBones()) {
			bones.resize(mesh->mNumBones);
			boneNameToId.clear();
			for(int b=0; b<mesh->mNumBones; ++b) {
				aiBone* bone = mesh->mBones[b];
				bones[b].name = bone->mName.C_Str();
				boneNameToId[bones[b].name] = b;
				bones[b].id = b;
				bones[b].globalInverseBindingPoseMatrix = ConvertAssimpToGlmMat(bone->mOffsetMatrix);
			}
			for(int b=0; b<mesh->mNumBones; ++b) {
				aiBone* bone = mesh->mBones[b];
				aiNode* node = scene->mRootNode->FindNode(bone->mName);
				
				const char* bname = node->mParent->mName.C_Str();
				if(boneNameToId.find(bname) != boneNameToId.end()) {
					bones[b].parentId = boneNameToId[bname];
				} else {
					bones[b].parentId = -1;
				}
			}
		}
	}
} // namespace BasicMeshLoader
} // namespace gl


