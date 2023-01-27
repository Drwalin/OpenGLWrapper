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

#ifndef OPEN_GL_BASIC_MESH_LOADER_MESH_HPP
#define OPEN_GL_BASIC_MESH_LOADER_MESH_HPP

#include <cinttypes>
#include <cmath>

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <string>
#include <memory>

#include <glm/glm.hpp>

class aiScene;
class aiMesh;

namespace gl {
namespace BasicMeshLoader {
	template<uint32_t dim>
	struct Value {
		float v[dim];
	};
	
	class Mesh {
	public:
		
		std::string name;
		
		std::vector<std::vector<Value<3>>> pos;			// always size == 1 ; for attributes compatibility reason with other attributes
		std::vector<std::vector<Value<2>>> uv;
		std::vector<std::vector<Value<4>>> color;
		std::vector<std::vector<Value<3>>> normal;		// always size == 1 ; for attributes compatibility reason with other attributes
		std::vector<std::vector<Value<1>>> weightBone;
		std::vector<std::vector<Value<1>>> weight;
		
		std::vector<uint32_t> indices;
		
		template<typename T>
		void AppendIndices(
				uint32_t vertexBaseOffsetWithData,
				std::vector<uint8_t>& elementBuffer);
				
		template<typename T, bool normalize, uint32_t dim>
		void ExtractAttribute(
				std::vector<std::vector<Value<dim>>> Mesh::* attribute,
				uint32_t baseOffset,
				std::vector<uint8_t>& buffer,
				uint32_t offset,
				uint32_t stride,
				uint32_t channelId = 0, // optional for uv/color/weights
				uint32_t subsequentChannels = 1
				) const;
		
		void LoadMesh(const aiMesh* mesh);
	};
} // namespace BasicMeshLoader
} // namespace gl

#endif

#include "Mesh.inl.hpp"

