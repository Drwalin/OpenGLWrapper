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
	bool AssimpLoader::Load(std::string file, LoaderFlagsBitfield flags) {
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
				void(*function)(AssimpLoader*, const aiNode*), bool reverse) {
		for(int i=0; i<node->mNumChildren; ++i) {
			const aiNode* n = node->mChildren[reverse?node->mNumChildren-1-i:i];
			if(reverse) {
				ForEachNode(n, function, reverse);
				function(this, n);
			} else {
				function(this, n);
				ForEachNode(n, function, reverse);
			}
		}
	}
	
	bool AssimpLoader::Load(const char* file, LoaderFlagsBitfield flags) {
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
		ConstructTransformationMatrix();
		
// 		PrintProperties();
		
		PrintNode(scene, scene->mRootNode, 0);
		
		meshNameToId.clear();
		meshes.clear();
		meshes.resize(s->mNumMeshes);
		for(int i=0; i<s->mNumMeshes; ++i) {
			meshes[i] = std::make_shared<Mesh>();
			meshes[i]->LoadMesh(this, s->mMeshes[i], flags);
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
				[](AssimpLoader*l, const aiNode*node) {
					std::string meshName, objectName;
					if(node->mNumMeshes > 0) {
						meshName = l->scene->mMeshes[node->mMeshes[0]]
							->mName.C_Str();
						objectName = node->mName.C_Str();
					}
					l->RenameMeshIfAvailable(objectName, meshName);
				}, false);
		}
		
		return true;
	}
	
	void AssimpLoader::RenameMeshIfAvailable(std::string oldName,
			std::string newName) {
		if(meshNameToId.find(newName) == meshNameToId.end()) {
			auto old = meshNameToId.find(oldName);
			if(old == meshNameToId.end()) {
				
			}
		}
	}
	
	void AssimpLoader::PrintProperties() {
		for(int i=0; i<scene->mMetaData->mNumProperties; ++i) {
			auto v = scene->mMetaData->mValues[i];
			printf(" '%s': ", scene->mMetaData->mKeys[i].C_Str());
			switch(v.mType) {
				case aiMetadataType::AI_AISTRING:
					printf("'%s'", (char*)v.mData);
					break;
				case aiMetadataType::AI_BOOL:
					printf("%s", *(int32_t*)v.mData?"TRUE":"FALSE");
					break;
				case aiMetadataType::AI_INT32:
					printf("%i", *(int32_t*)v.mData);
					break;
				case aiMetadataType::AI_UINT64:
					printf("%lu", *(uint64_t*)v.mData);
					break;
				case aiMetadataType::AI_FLOAT:
					printf("%f", *(float*)v.mData);
					break;
				case aiMetadataType::AI_DOUBLE:
					printf("%f", *(double*)v.mData);
					break;
				case aiMetadataType::AI_AIVECTOR3D: {
						float* l = (float*)v.mData;
						printf("{%f , %f , %f}", l[0], l[1], l[2]);
					} break;
				default:
					printf("other type");
			}
			printf("\n");
		}
	}
	
	void AssimpLoader::ConstructTransformationMatrix() {
		rootTransformationMatrix = glm::mat4(1);
		
		// blender orientation correction
		{
			int up, ups, oup, oups, f, fs, c, cs;
			if(!scene->mMetaData->Get<int>("UpAxis", up))
				return;
			if(!scene->mMetaData->Get<int>("UpAxisSign", ups))
				return;
			if(!scene->mMetaData->Get<int>("OriginalUpAxis", oup))
				return;
			if(!scene->mMetaData->Get<int>("OriginalUpAxisSign", oups))
				return;
			if(!scene->mMetaData->Get<int>("FrontAxis", f))
				return;
			if(!scene->mMetaData->Get<int>("FrontAxisSign", fs))
				return;
			if(!scene->mMetaData->Get<int>("CoordAxis", c))
				return;
			if(!scene->mMetaData->Get<int>("CoordAxisSign", cs))
				return;

			if(up != 1
					&& ups != 1
					&& oup != -1
					&& oups != 1
					&& f != 2
					&& fs != 1
					&& c != 0
					&& cs != 1)
				return;

			rootTransformationMatrix =
				glm::mat4(
						{1,0,0,0},
						{0,0,-1,0},
						{0,1,0,0},
						{0,0,0,1});
		}
	}
	
} // namespace BasicMeshLoader
} // namespace gl

