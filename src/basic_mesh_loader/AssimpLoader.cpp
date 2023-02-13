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

#include "../../include/openglwrapper/basic_mesh_loader/AssimpLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

namespace gl {
namespace BasicMeshLoader {
	void AssimpLoader::Load(const char* file) {
		importer = std::make_shared<Assimp::Importer>();
		const ::aiScene* s = importer->ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		scene = s;
		
		meshNameToId.clear();
		meshes.clear();
		meshes.resize(s->mNumMeshes);
		for(int i=0; i<s->mNumMeshes; ++i) {
			meshes[i] = std::make_shared<Mesh>();
			meshes[i]->LoadMesh(s, s->mMeshes[i]);
			meshNameToId.emplace(meshes[i]->name, i);
		}
		
		/*
		skeletonNameToId.clear();
		skeletons.clear();
		skeletons.resize(s->mNumSkeletons);
		for(int i=0; i<s->mNumSkeletons; ++i) {
			skeletons[i] = std::make_shared<Skeleton>();
			skeletons[i]->LoadSkeleton(s->mSkeletons[i]);
			skeletonNameToId.emplace(skeletons[i]->name, i);
		}
		*/
		
		animationNameToId.clear();
		animations.clear();
		animations.resize(s->mNumAnimations);
		for(int i=0; i<s->mNumAnimations; ++i) {
			animations[i] = std::make_shared<Animation>();
			animations[i]->LoadAnimation(this, s->mAnimations[i], meshes[0]);
			animationNameToId.emplace(animations[i]->name, i);
		}
	}
} // namespace BasicMeshLoader
} // namespace gl

