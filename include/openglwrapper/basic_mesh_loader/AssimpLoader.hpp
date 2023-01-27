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

#ifndef OPEN_GL_BASIC_MESH_LOADER_ASSIMP_LOADER_HPP
#define OPEN_GL_BASIC_MESH_LOADER_ASSIMP_LOADER_HPP

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

class aiScene;
class aiMesh;

namespace gl {
namespace BasicMeshLoader {
	
	class AssimpLoader {
	public:
		std::vector<std::shared_ptr<Mesh>> meshes;
// 		std::vector<std::shared_ptr<Skeleton>> skeletons;
// 		std::vector<std::shared_ptr<Animation>> animations;
		
		void Load(const char* file);
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

#include "AssimpLoader.inl.hpp"

