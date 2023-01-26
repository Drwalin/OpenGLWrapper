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

#include <openglwrapper/AssimpLoader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace gl {
	void AssimpLoader::Mesh::LoadMesh(const aiMesh* mesh) {
		uint32_t vertices = mesh->mNumVertices;
		uint32_t colors = mesh->GetNumColorChannels();
		uint32_t uvs = mesh->GetNumUVChannels();
		
		
		
		
		
		if(mesh->HasPositions()) {
			pos.resize(1);
			pos[0].resize(vertices);
			for(int i=0; i<mesh->mNumVertices; ++i) {
				pos[0][i] = {
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
			normal.resize(1);
			normal[0].resize(vertices);
			for(int i=0; i<mesh->mNumVertices; ++i) {
				normal[0][i] = {
						mesh->mNormals[i].x,
						mesh->mNormals[i].y,
						mesh->mNormals[i].z
				};
			}
		}
		
		if(mesh->HasBones()) {
			const int weightsPerVert = 4;
			weight.resize(weightsPerVert);
			weightBone.resize(weightsPerVert);
			for(int i=0; i<weightsPerVert; ++i) {
				weight[i].clear();
				weight[i].resize(vertices, {0});
				weightBone[i].clear();
				weightBone[i].resize(vertices, {0});
			}
			for(int b=0; b<mesh->mNumBones; ++b) {
				aiBone* bone = mesh->mBones[b];
				for(int i=0; i<bone->mNumWeights; ++i) {
					aiVertexWeight W = bone->mWeights[i];
					uint32_t v = W.mVertexId;
					float w = W.mWeight;
					uint32_t minId = 3;
					float min = weight[3][v].v[0];
					for(int j=3; j>=0; --j) {
						if(min >= weight[j][v].v[0]) {
							min = weight[j][v].v[0];
							minId = j;
						}
					}
					if(min < w) {
						weight[minId][v].v[0] = w;
						weightBone[minId][v].v[0] = b;
					}
				}
			}
		}
		
		for(int i=0; i<mesh->mNumFaces; ++i) {
			auto f = mesh->mFaces[i];
			for(int j=2; j<f.mNumIndices; ++j) {
				indices.emplace_back(f.mIndices[0]);
				indices.emplace_back(f.mIndices[j-1]);
				indices.emplace_back(f.mIndices[j]);
			}
		}
		
		this->name = mesh->mName.C_Str();
	}
	
	void AssimpLoader::Load(const char* file) {
		Assimp::Importer importer;
		const ::aiScene* s = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		auto m = s->mMeshes[0];
		
		this->meshes.clear();
		this->meshes.resize(s->mNumMeshes);
		for(int i=0; i<s->mNumMeshes; ++i) {
			meshes[i].LoadMesh(s->mMeshes[i]);
		}
		
		
		
		pos.resize(m->mNumVertices);
		color.resize(m->mNumVertices);
		uv.resize(m->mNumVertices);
		norm.resize(m->mNumVertices);
		indices.reserve(m->mNumFaces*3);
		
		if(m->HasPositions()) {
			for(int i=0; i<m->mNumVertices; ++i) {
				pos[i] = glm::vec3(
						m->mVertices[i].x,
						m->mVertices[i].y,
						m->mVertices[i].z
						);
			}
		}
		
		if(m->HasVertexColors(0)) {
			for(int i=0; i<m->mNumVertices; ++i) {
				color[i] = glm::vec4(
						m->mColors[0][i].r,
						m->mColors[0][i].g,
						m->mColors[0][i].b,
						m->mColors[0][i].a
						);
			}
		}
		
		if(m->GetNumUVChannels() > 0) {
			for(int i=0; i<m->mNumVertices; ++i) {
				uv[i] = glm::vec2(
						m->mTextureCoords[0][i].x,
						m->mTextureCoords[0][i].y
						);
			}
		}
		
		if(m->HasNormals()) {
			for(int i=0; i<m->mNumVertices; ++i) {
				norm[i] = glm::vec3(
						m->mNormals[i].x,
						m->mNormals[i].y,
						m->mNormals[i].z
						);
			}
		}
		
		for(int i=0; i<m->mNumFaces; ++i) {
			auto f = m->mFaces[i];
			for(int j=2; j<f.mNumIndices; ++j) {
				indices.emplace_back(f.mIndices[0]);
				indices.emplace_back(f.mIndices[j-1]);
				indices.emplace_back(f.mIndices[j]);
			}
		}
		
		// weights of vertices of bone [0]
		s->mSkeletons[0]->mBones[0]->mWeights;
		
		// animations data probrably per bone
		s->mAnimations[0]->mChannels[0]->mPositionKeys;
		s->mAnimations[0]->mChannels[0]->mRotationKeys;
		s->mAnimations[0]->mChannels[0]->mScalingKeys;
	}
} // namespace gl

