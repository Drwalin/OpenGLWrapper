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

#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vector_relational.hpp>
#include <glm/mat4x4.hpp>

#include "../../include/openglwrapper/basic_mesh_loader/AssimpLoader.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/Skeleton.hpp"

#include "../../include/openglwrapper/basic_mesh_loader/Mesh.hpp"

namespace gl {
namespace BasicMeshLoader {
	
	template<>
	glm::mat4 ConvertAssimpToGlmMat<aiMatrix4x4>(aiMatrix4x4 s) {
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

	void print(glm::vec4 v) {
		printf("{%f %f %f %f}", v.x, v.y, v.z, v.w);
	}

	void print(glm::vec3 v) {
		printf("{%f %f %f}", v.x, v.y, v.z);
	}

	void print(glm::quat v) {
		printf("{%f %f %f %f}", v.x, v.y, v.z, v.w);
	}

	void printMat(glm::mat4 m) {
		printf("{\n");
		for(int i=0; i<4; ++i) {
			for(int j=0; j<4; ++j) {
				printf(" %+10.3f", m[j][i]);
			}
			printf("\n");
		}
		printf("}");
	}
	
	
	
	void Mesh::MergeWithOtherMesh(std::shared_ptr<Mesh> otherMesh) {
		throw "gl::BasicMeshLoader::Mesh::MergeWithOtherMesh is not implemented.";
	}
	
	
	
	void Mesh::GetBoundingSphereInfo(float* center, float& radius) {
		center[0] = boundingSphereCenter[0];
		center[1] = boundingSphereCenter[1];
		center[2] = boundingSphereCenter[2];
		radius = boundingSphereRadius;
	}
	
	int Mesh::GetBoneIndex(std::string boneName) {
		if(skeleton.get()) {
			return skeleton->GetBoneIndex(boneName);
		}
		return -1;
	}
	
	void Mesh::LoadMesh(class AssimpLoader* loader, const aiMesh* mesh,
			LoaderFlagsBitfield flags) {
		uint32_t vertices = mesh->mNumVertices;
		uint32_t colors = mesh->GetNumColorChannels();
		uint32_t uvs = mesh->GetNumUVChannels();
		
		if(mesh->HasPositions()) {
			pos.resize(vertices);
			for(int i=0; i<mesh->mNumVertices; ++i) {
				glm::vec3 vvv = {
					mesh->mVertices[i].x,
					mesh->mVertices[i].y,
					mesh->mVertices[i].z
				};
				if(!mesh->HasBones()) {
					if(flags & CORRECT_NOT_ANIMATED_MESH_ORIENTATION) {
						vvv = loader->rootTransformationMatrix
							* glm::vec4(vvv, 1);
					}
				}
				pos[i] = {
					vvv.x,
					vvv.y,
					vvv.z
				};
			}
			boundingBoxMax = boundingBoxMin = pos[0];
			for(glm::vec3 v : pos) {
				boundingBoxMin.x = std::min(boundingBoxMin.x, v.x);
				boundingBoxMin.y = std::min(boundingBoxMin.y, v.y);
				boundingBoxMin.z = std::min(boundingBoxMin.z, v.z);
				boundingBoxMax.x = std::max(boundingBoxMax.x, v.x);
				boundingBoxMax.y = std::max(boundingBoxMax.y, v.y);
				boundingBoxMax.z = std::max(boundingBoxMax.z, v.z);
			}
			
			if(!mesh->HasBones()) {
				boundingSphereCenter = (boundingBoxMin + boundingBoxMax) * 0.5f;
				float r2 = 0;
				for(glm::vec3 v : pos) {
					r2 = std::max(r2, glm::dot(v-boundingSphereCenter,
								v-boundingSphereCenter));
				}
				boundingSphereRadius = sqrt(r2);
			} else {
				boundingSphereCenter = {0,0,0};
				float r2 = 0;
				for(glm::vec3 v : pos) {
					r2 = std::max(r2, glm::dot(v-boundingSphereCenter,
								v-boundingSphereCenter));
				}
				boundingSphereRadius = sqrt(r2)*1.5;
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
				glm::vec3 vvv = {
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
				if(!mesh->HasBones()) {
					if(flags & CORRECT_NOT_ANIMATED_MESH_ORIENTATION) {
						vvv = loader->rootTransformationMatrix
							* glm::vec4(vvv, 0);
					}
				}
				normal[i] = {
					vvv.x,
					vvv.y,
					vvv.z
				};
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
		
		if(mesh->HasBones()) {
			skeleton = std::make_shared<Skeleton>();
			skeleton->LoadSkeleton(this, mesh, loader->scene);
		
			weight.resize(vertices);
			for(int b=0; b<mesh->mNumBones; ++b) {
				aiBone* bone = mesh->mBones[b];
				for(int i=0; i<bone->mNumWeights; ++i) {
					aiVertexWeight w = bone->mWeights[i];
					weight[w.mVertexId].emplace_back(VertexBoneWeight(b, w.mWeight));
				}
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
	}
} // namespace BasicMeshLoader
} // namespace gl

