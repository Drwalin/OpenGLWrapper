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

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vector_relational.hpp>
#include <glm/mat4x4.hpp>

static glm::mat4 ConvertAssimpToGlmMat(aiMatrix4x4 s) {
	aiVector3D pos, scale;
	aiQuaternion rot;
	s.Decompose(scale, rot, pos);
	
	glm::mat4 ss = glm::scale(glm::mat4(1), {scale.x, scale.y, scale.z});
	glm::mat4 pp = glm::translate(glm::mat4(1), {pos.x, pos.y, pos.z});
	glm::mat4 rr = glm::mat4_cast(glm::quat(rot.w, rot.x, rot.y, rot.z));
	
	return pp * rr * ss;
	
	
	return glm::transpose(glm::mat4(
			s.a1, s.a2, s.a3, s.a4,
			s.b1, s.b2, s.b3, s.b4,
			s.c1, s.c2, s.c3, s.c4,
			s.d1, s.d2, s.d3, s.d4));
}

static void print(glm::vec4	v) {
	printf("{%f %f %f %f}", v.x, v.y, v.z, v.w);
}

static void print(glm::vec3 v) {
	printf("{%f %f %f}", v.x, v.y, v.z);
}

static void print(glm::quat v) {
	printf("{%f %f %f %f}", v.x, v.y, v.z, v.w);
}

template<typename T>
static void printKey(T v) {
	printf(" key: %f -> ", v.time);
	print(v.value);
	printf("\n");
}

static void printMat(glm::mat4 m) {
	printf("{\n");
	for(int i=0; i<4; ++i) {
		for(int j=0; j<4; ++j) {
			printf(" %+10.3f", m[j][i]);
		}
		printf("\n");
	}
	printf("}");
}

namespace gl {
namespace BasicMeshLoader {
	
	int Mesh::GetBoneIndex(std::string boneName) {
		auto it = boneNameToId.find(boneName);
		if(it == boneNameToId.end())
			return -1;
		return it->second;
	}
	
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
		
		rootInverseMatrix = glm::inverse(ConvertAssimpToGlmMat(scene->mRootNode->mTransformation));
		
		if(mesh->HasBones()) {
			weight.resize(vertices);
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
						[](VertexBoneWeight a, VertexBoneWeight b)->bool{
							return a.weight > b.weight;
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

