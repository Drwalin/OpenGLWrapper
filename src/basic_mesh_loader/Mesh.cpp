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

#include <openglwrapper/basic_mesh_loader/Mesh.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace gl {
namespace BasicMeshLoader {
	void Mesh::LoadMesh(const aiMesh* mesh) {
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
} // namespace BasicMeshLoader
} // namespace gl

