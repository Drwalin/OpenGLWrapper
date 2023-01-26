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

#define DEBUG {printf(" :%i\n", __LINE__); fflush(stdout);}

#include <openglwrapper/AssimpLoader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace gl {
	void AssimpLoader::Load(const char* file) {
		Assimp::Importer importer;
		auto s = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
// 		int numMesh = s->mNumMeshes;
		auto m = s->mMeshes[0];
		
		printf(" num meshes = %i\n", s->mNumMeshes);
		
		pos.resize(m->mNumVertices);
		color.resize(m->mNumVertices);
		uv.resize(m->mNumVertices);
		norm.resize(m->mNumVertices);
		indices.reserve(m->mNumFaces*3);
		
		DEBUG;
		if(m->HasPositions()) {
		DEBUG;
			for(int i=0; i<m->mNumVertices; ++i) {
			if(!i)
		DEBUG;
				pos[i] = glm::vec3(
						m->mVertices[i].x,
						m->mVertices[i].y,
						m->mVertices[i].z
						);
			}
		}
		
		DEBUG;
		if(m->HasVertexColors(0)) {
		DEBUG;
			for(int i=0; i<m->mNumVertices; ++i) {
			if(!i)
		DEBUG;
				color[i] = glm::vec4(
						m->mColors[0][i].r,
						m->mColors[0][i].g,
						m->mColors[0][i].b,
						m->mColors[0][i].a
						);
			}
		}
		
		DEBUG;
		if(m->GetNumUVChannels() > 0) {
		DEBUG;
			for(int i=0; i<m->mNumVertices; ++i) {
			if(!i)
		DEBUG;
				uv[i] = glm::vec2(
						m->mTextureCoords[0][i].x,
						m->mTextureCoords[0][i].y
						);
			}
		}
		
		DEBUG;
		if(m->HasNormals()) {
		DEBUG;
			for(int i=0; i<m->mNumVertices; ++i) {
			if(!i)
		DEBUG;
				norm[i] = glm::vec3(
						m->mNormals[i].x,
						m->mNormals[i].y,
						m->mNormals[i].z
						);
			}
		}
		
		DEBUG;
		for(int i=0; i<m->mNumFaces; ++i) {
			if(!i)
		DEBUG;
			auto f = m->mFaces[i];
			for(int j=2; j<f.mNumIndices; ++j) {
// 			int j = 2;
			if(!i)
		DEBUG;
				indices.emplace_back(f.mIndices[0]);
				indices.emplace_back(f.mIndices[j-1]);
				indices.emplace_back(f.mIndices[j]);
			}
		}
	}
} // namespace gl

