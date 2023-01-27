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
		
// 		// weights of vertices of bone [0]
// 		s->mSkeletons[0]->mBones[0]->mWeights;
// 		
// 		// animations data probrably per bone
// 		s->mAnimations[0]->mChannels[0]->mPositionKeys;
// 		s->mAnimations[0]->mChannels[0]->mRotationKeys;
// 		s->mAnimations[0]->mChannels[0]->mScalingKeys;
	}
} // namespace BasicMeshLoader
} // namespace gl

