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
		
		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> norm;
		std::vector<glm::vec4> color;
		std::vector<glm::vec2> uv;
		
		std::vector<uint32_t> indices;
		
		void Load(const char* file);
		
		
		
		/*
		inline const static std::vector<std::vector<Value<3>>> Mesh::* ATTR_POS = &Mesh::pos;
		inline const static std::vector<std::vector<Value<2>>> Mesh::* ATTR_UV = &Mesh::uv;
		inline const static std::vector<std::vector<Value<4>>> Mesh::* ATTR_COLOR = &Mesh::color;
		inline const static std::vector<std::vector<Value<3>>> Mesh::* ATTR_NORMAL = &Mesh::normal;
		inline const static std::vector<std::vector<Value<1>>> Mesh::* ATTR_WEIGHT_BONE = &Mesh::weightBone;
		inline const static std::vector<std::vector<Value<1>>> Mesh::* ATTR_WEIGHT = &Mesh::weight;
		
		template<uint32_t dim>
		constexpr inline static uint32_t GetBasicElementsOfAttribute(std::vector<std::vector<Value<dim>>> Mesh::* attribute);
		
		template<typename T, bool normalize, bool negative, uint32_t dim>
		inline static void Convert(T* dst, Value<dim> value);
		*/
		
		
		template<typename T, uint32_t dim>
		inline static void Copy(
				uint32_t dstOffset,
				std::vector<uint8_t>& dst,
				uint32_t offset,
				uint32_t stride,
				const std::vector<Value<dim>>& src,
				void(*converter)(T* dst, Value<dim> value));
		
		template<typename T, uint32_t dim>
		static void ConverterFloatPlain(T* dst, Value<dim> value);
		
		template<typename T, uint32_t dim>
		static void ConverterFloatNormalized(T* dst, Value<dim> value);
		
		template<typename T, uint32_t dim>
		static void ConverterIntPlain(T* dst, Value<dim> value);
		
		template<typename T, T min, T max, uint32_t dim>
		static void ConverterIntPlainClamp(T* dst, Value<dim> value);
		
		template<typename T, T len, uint32_t dim>
		static void ConverterIntNormalized(T* dst, Value<dim> value);
		
		template<typename T, T mid, T max_len, uint32_t dim>
		static void ConverterUnsignedNormalized(T* dst, Value<dim> value);
		
		template<uint32_t dim>
		inline static Value<dim> Normalize(Value<dim> value);
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

#include "AssimpLoader.inl.hpp"

