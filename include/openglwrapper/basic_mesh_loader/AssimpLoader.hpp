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

#pragma once

#ifndef OGLW_BASIC_MESH_LOADER_ASSIMP_LOADER_HPP
#define OGLW_BASIC_MESH_LOADER_ASSIMP_LOADER_HPP

#include <cinttypes>
#include <cmath>

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "Value.hpp"
#include "Mesh.hpp"
#include "Skeleton.hpp"
#include "Animation.hpp"

class aiScene;
class aiMesh;
class aiNode;

namespace Assimp {
	class Importer;
}

namespace gl {
namespace BasicMeshLoader {
	
	enum LoadingFlags : uint64_t {
		NONE = 0,
		RENAME_MESH_BY_FIRST_NODE_WITH_ITS_NAME = 1,
	};
	
	class AssimpLoader {
	public:
		
		void ForEachNode(const aiNode* node,
				void(*function)(const aiScene*, const aiNode*),
				bool reverse=false);
		
		std::shared_ptr<Assimp::Importer> importer;
		aiScene const* scene;
		
		std::unordered_map<std::string, uint32_t> meshNameToId;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::unordered_map<std::string, uint32_t> skeletonNameToId;
		std::vector<std::shared_ptr<Skeleton>> skeletons;
		std::unordered_map<std::string, uint32_t> animationNameToId;
		std::vector<std::shared_ptr<Animation>> animations;
		
		bool Load(const char* file, LoadingFlags flags = NONE);
		bool Load(std::string file, LoadingFlags flags = NONE);
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

#include "AssimpLoader.inl.hpp"

