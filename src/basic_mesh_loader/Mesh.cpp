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
			this->weight.resize(vertices);
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

