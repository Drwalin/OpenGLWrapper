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

#ifndef OPEN_GL_BASIC_MESH_LOADER_SKELETON_HPP
#define OPEN_GL_BASIC_MESH_LOADER_SKELETON_HPP

#include <cinttypes>
#include <cmath>

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <string>
#include <memory>

#include <glm/glm.hpp>

class aiSkeleton;
class aiScene;
class aiMesh;

namespace gl {
namespace BasicMeshLoader {
	
	class Mesh;
	
	class Bone {
	public:
		std::string name;
		glm::mat4 globalInverseBindingPoseMatrix;
		
		int id;
		int parentId;
	};
	
	class Skeleton {
	public:
		
		std::string name;
		
		std::vector<Bone> bones;
		std::unordered_map<std::string, int32_t> boneNameToId;
		glm::mat4 inverseGlobalMatrix;
		glm::mat4 globalMatrix;
		glm::mat4 rootInverseMatrix;
		
		int GetBoneIndex(std::string boneName);
		void LoadSkeleton(Mesh* cmesh, const aiMesh* aimesh, const aiScene* scene);
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

