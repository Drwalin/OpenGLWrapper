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
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include "../../include/openglwrapper/basic_mesh_loader/Mesh.hpp"

static glm::mat4 ConvertAssimpToGlmMat(aiMatrix4x4 s) {
	return glm::mat4(
			s.a1, s.a2, s.a3, s.a4,
			s.b1, s.b2, s.b3, s.b4,
			s.c1, s.c2, s.c3, s.c4,
			s.d1, s.d2, s.d3, s.d4);
}

namespace gl {
namespace BasicMeshLoader {
	void Mesh::LoadMesh(const aiScene* scene, const aiMesh* mesh) {
		uint32_t vertices = mesh->mNumVertices;
		uint32_t colors = mesh->GetNumColorChannels();
		uint32_t uvs = mesh->GetNumUVChannels();
		
		if(mesh->HasPositions()) {
			pos.resize(vertices);
			for(int i=0; i<mesh->mNumVertices; ++i) {
				pos[i] = {
						mesh->mVertices[i].x,
						mesh->mVertices[i].y,
						mesh->mVertices[i].z
				};
			}
		}
		
		if(mesh->HasVertexColors(0)) {
			color.resize(colors);
			for(int j=0; j<colors; ++j) {
				color[j].resize(vertices);
				for(int i=0; i<mesh->mNumVertices; ++i) {
					color[j][i] = {
							mesh->mColors[j][i].r,
							mesh->mColors[j][i].g,
							mesh->mColors[j][i].b,
							mesh->mColors[j][i].a
					};
				}
			}
		}
		
		if(mesh->GetNumUVChannels() > 0) {
			uv.resize(uvs);
			for(int j=0; j<uvs; ++j) {
				uv[j].resize(vertices);
				for(int i=0; i<mesh->mNumVertices; ++i) {
					uv[j][i] = {
							mesh->mTextureCoords[j][i].x,
							mesh->mTextureCoords[j][i].y
					};
				}
			}
		}
		
		if(mesh->HasNormals()) {
			normal.resize(vertices);
			for(int i=0; i<mesh->mNumVertices; ++i) {
				normal[i] = {
						mesh->mNormals[i].x,
						mesh->mNormals[i].y,
						mesh->mNormals[i].z
				};
			}
		}
		
		if(mesh->HasBones()) {
			weight.resize(vertices);
			bones.resize(mesh->mNumBones);
			boneNameToId.clear();
			for(int b=0; b<mesh->mNumBones; ++b) {
				aiBone* bone = mesh->mBones[b];
				bones[b].name = bone->mName.C_Str();
				boneNameToId[bones[b].name] = b;
				bones[b].id = b;
			}
			for(int b=0; b<mesh->mNumBones; ++b) {
				aiBone* bone = mesh->mBones[b];
				
				aiNode* node = scene->mRootNode->FindNode(bone->mName);
				
				const char* bname = node->mParent->mName.C_Str();
				if(boneNameToId.find(bname) != boneNameToId.end()) {
					bones[b].parentId = boneNameToId[bname];
				} else {
					aiNode* cn = node->mParent;// should start here, or a in nodeo; ??
					glm::mat4 mat(1.0f);
					while(cn) {
						glm::mat4 m = ConvertAssimpToGlmMat(cn->mTransformation);
						mat = m * mat;
						cn = cn->mParent;
					}
					
					inverseGlobalMatrix = glm::inverse(mat);
					bones[b].parentId = -1;
				}
				
				printf(" %i -> %i ; %s -> %s   ((%i)) \n", bones[b].parentId, bones[b].id, bones[b].parentId > 0 ? bones[bones[b].parentId].name.c_str():"[nil]", bones[b].name.c_str(), bone->mNumWeights);
				
				bones[b].globalInverseBindingPoseMatrix = ConvertAssimpToGlmMat(bone->mOffsetMatrix);
				bones[b].relativePosition = ConvertAssimpToGlmMat(node->mTransformation);
				
				for(int i=0; i<bone->mNumWeights; ++i) {
					aiVertexWeight w = bone->mWeights[i];
					weight[w.mVertexId].emplace_back(VertexBoneWeight(b, w.mWeight));
				}
			}
			for(int b=0; b<mesh->mNumBones; ++b) {
				glm::mat4 fwd = glm::inverse(bones[b].globalInverseBindingPoseMatrix);
				fwd = inverseGlobalMatrix * fwd;
				bones[b].inverseLocalModelSpaceBindingPoseMatrix = glm::inverse(fwd);
			}
			for(int i=0; i<vertices; ++i) {
				std::sort(
						weight[i].begin(),
						weight[i].end(),
						[](VertexBoneWeight a, VertexBoneWeight b)->int{
							if(a.weight < b.weight)
								return -1;
							if(a.weight > b.weight)
								return 1;
							return 0;
						});
			}
		}
		
		for(int i=0; i<mesh->mNumFaces; ++i) {
			const auto& f = mesh->mFaces[i];
			for(int j=2; j<f.mNumIndices; ++j) {
				indices.emplace_back(f.mIndices[0]);
				indices.emplace_back(f.mIndices[j-1]);
				indices.emplace_back(f.mIndices[j]);
			}
		}
		
		this->name = mesh->mName.C_Str();
	}
} // namespace BasicMeshLoader
} // namespace gl

