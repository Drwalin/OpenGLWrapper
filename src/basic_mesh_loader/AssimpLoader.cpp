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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

#include "../../include/openglwrapper/basic_mesh_loader/AssimpLoader.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/Mesh.hpp"

namespace gl {
namespace BasicMeshLoader {
	bool AssimpLoader::Load(std::string file, LoadingFlags flags) {
		return Load(file.c_str(), flags);
	}
	
	void PrintNode(const aiScene* scene, const aiNode* node, const int depth) {
		for(int i=0; i<depth; ++i) {
			printf("    ");
		}
		printf("'%s':\n", node->mName.C_Str());
		for(int i=0; i<node->mNumMeshes; ++i) {
			for(int i=0; i<=depth; ++i) printf("    ");
			printf("   :: mesh '%s'\n", scene->mMeshes[node->mMeshes[i]]->mName.C_Str());
		}
		for(int i=0; i<node->mNumChildren; ++i) {
			PrintNode(scene, node->mChildren[i], depth+1);
		}
	}
	
	void AssimpLoader::ForEachNode(const aiNode* node,
				void(*function)(const aiScene*, const aiNode*), bool reverse) {
		for(int i=0; i<node->mNumChildren; ++i) {
			const aiNode* n = node->mChildren[reverse?node->mNumChildren-1-i:i];
			if(reverse) {
			} else {
				ForEachNode(n, function, reverse);

			}
		}
	}
	
	bool AssimpLoader::Load(const char* file, LoadingFlags flags) {
		importer = std::make_shared<Assimp::Importer>();
		const ::aiScene* s = importer->ReadFile(file, aiProcess_Triangulate
				| aiProcess_JoinIdenticalVertices
				| aiProcess_CalcTangentSpace
				| aiProcess_GenSmoothNormals
				| aiProcess_ImproveCacheLocality
				| aiProcess_RemoveRedundantMaterials
				);
		if(s == nullptr)
			return false;
		scene = s;
		
		PrintNode(scene, scene->mRootNode, 0);
		
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
		
		if(RENAME_MESH_BY_FIRST_NODE_WITH_ITS_NAME & flags) {
			ForEachNode(scene->mRootNode,
				[](const aiScene*scene, const aiNode*node){
					
				}, true);
		}
		
		return true;
	}
} // namespace BasicMeshLoader
} // namespace gl

