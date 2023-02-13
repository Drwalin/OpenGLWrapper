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
		rootInverseMatrix = glm::inverse(ConvertAssimpToGlmMat(scene->mRootNode->mTransformation));
		
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
					
					bones[b].relativePosition = 
						bones[bones[b].parentId].globalInverseBindingPoseMatrix
						* glm::inverse(bones[b].globalInverseBindingPoseMatrix)
						;
				} else {
					aiNode* cn = node;//->mParent;// should start here, or a in nodeo; ??
					glm::mat4 mat(1.0f);
					while(cn) {
						glm::mat4 m = ConvertAssimpToGlmMat(cn->mTransformation);
						mat = m * mat;
						printf(" Multiplying from top by (%s):\n", cn->mName.C_Str());
						printMat(m);
						printf("\n\n");
						cn = cn->mParent;
					}
					
					globalMatrix = mat;
					inverseGlobalMatrix = glm::inverse(mat);
					bones[b].parentId = -1;
					
					printf("\n\n\n");
// 					printf("Inverse of root binding matrix:\n");
// 					printMat(glm::inverse(bones[b].globalInverseBindingPoseMatrix));
// 					printf("\n\n\n\n\n");
					
					
					
					bones[b].relativePosition = mat;
// 					bones[b].relativePosition = glm::mat4(1);
				}
				
				printf("Bone relative (%s):\n", bones[b].name.c_str());
				printMat(bones[b].relativePosition);
				printf("\n\n");
			}
			for(int b=0; b<mesh->mNumBones; ++b) {
				glm::mat4 fwd = glm::inverse(bones[b].globalInverseBindingPoseMatrix);
				fwd = inverseGlobalMatrix * fwd;
				bones[b].inverseLocalModelSpaceBindingPoseMatrix = glm::inverse(fwd);
			}
		}
	}
} // namespace BasicMeshLoader
} // namespace gl


