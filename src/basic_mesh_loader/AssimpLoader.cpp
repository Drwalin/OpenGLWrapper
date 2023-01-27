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

#include <openglwrapper/basic_mesh_loader/AssimpLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace gl {
namespace BasicMeshLoader {
	void AssimpLoader::Load(const char* file) {
		Assimp::Importer importer;
		const ::aiScene* s = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		auto m = s->mMeshes[0];
		
		this->meshes.clear();
		this->meshes.resize(s->mNumMeshes);
		for(int i=0; i<s->mNumMeshes; ++i) {
			meshes[i] = std::make_shared<Mesh>();
			meshes[i]->LoadMesh(s->mMeshes[i]);
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
} // namespace BasicMeshLoader
} // namespace gl

