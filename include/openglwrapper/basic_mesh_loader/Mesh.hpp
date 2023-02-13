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
#include <limits>

#include <glm/glm.hpp>

#include "Value.hpp"
#include "Skeleton.hpp"

class aiScene;
class aiMesh;

namespace gl {
namespace BasicMeshLoader {
	
	template<typename T>
	glm::mat4 ConvertAssimpToGlmMat(T s);
	void print(glm::vec4 v);
	void print(glm::vec3 v);
	void print(glm::quat v);
	void printMat(glm::mat4 m);
	inline void print(glm::mat4 m) {printMat(m);}

	template<typename T>
		void printKey(T v) {
			printf(" key: %f -> ", v.time);
			print(v.value);
			printf("\n");
		}
	
	
	struct VertexBoneWeight {
	public:
		inline VertexBoneWeight(uint32_t boneId=0, float weight=0) : boneId(boneId), weight(weight) {}
		
		uint32_t boneId;
		float weight;
	};
	
	class MeshBone {
	public:
		std::string name;
		glm::mat4 inverseLocalModelSpaceBindingPoseMatrix;
		glm::mat4 globalInverseBindingPoseMatrix;
		glm::mat4 relativePosition;
		
		glm::mat4 handCalculatedBindingPoseMatrix;
		
		int id;
		int parentId;
	};
	
	
	class Mesh {
	public:
		
		std::string name;
		
		std::vector<Value<3>> pos;
		std::vector<std::vector<Value<2>>> uv;
		std::vector<std::vector<Value<4>>> color;
		std::vector<Value<3>> normal;
		std::vector<std::vector<VertexBoneWeight>> weight;
		
		std::vector<uint32_t> indices;
		
// 		// begin skeleton
// 		std::vector<MeshBone> bones;
// 		std::unordered_map<std::string, int32_t> boneNameToId;
// 		glm::mat4 inverseGlobalMatrix;
// 		glm::mat4 globalMatrix;
// 		glm::mat4 rootInverseMatrix;
// 		// end skeleton
		
		std::shared_ptr<Skeleton> skeleton;
		
		
		int GetBoneIndex(std::string boneName);
		
		
		void LoadMesh(const aiScene* scene, const aiMesh* mesh);
		
		
		template<typename T>
		void AppendIndices(
				uint32_t vertexBaseOffsetWithData,
				std::vector<uint8_t>& elementBuffer
				) const;
		

		template<typename T>
		void ExtractPos(
				uint32_t baseOffset, // bytes
				std::vector<uint8_t>& buffer,
				uint32_t offset,
				uint32_t stride,
				void(*converter)(T* dst, Value<3> value) = ConverterFloatPlain<T, 3>
				) const;

		template<typename T>
		void ExtractNormal(
				uint32_t baseOffset, // bytes
				std::vector<uint8_t>& buffer,
				uint32_t offset,
				uint32_t stride,
				void(*converter)(T* dst, Value<3> value)
				) const;
				
		template<typename T>
		void ExtractUV(
				uint32_t baseOffset,
				std::vector<uint8_t>& buffer,
				uint32_t offset,
				uint32_t stride,
				void(*converter)(T* dst, Value<2> value),
				uint32_t channelId = 0,
				uint32_t subsequentChannels = 1
				) const;
				
		template<typename T>
		void ExtractColor(
				uint32_t baseOffset,
				std::vector<uint8_t>& buffer,
				uint32_t offset,
				uint32_t stride,
				void(*converter)(T* dst, Value<4> value)
					= ConverterIntPlainClampScale<
						T,
						std::numeric_limits<T>::max(),
						std::numeric_limits<T>::min(),
						std::numeric_limits<T>::max(),
						4>,
				uint32_t channelId = 0,
				uint32_t subsequentChannels = 1
				) const;
				
		template<typename Tweight, typename Tbone>
		void ExtractWeightsWithBones(
				uint32_t baseOffset,
				std::vector<uint8_t>& buffer,
				uint32_t weightsOffset,
				uint32_t boneIdsOffset,
				uint32_t stride,
				void(*converterWeight)(Tweight* dst, Value<1> value),
				uint32_t weightsCount = 4
				) const;
	};
} // namespace BasicMeshLoader
} // namespace gl

#endif

#include "Mesh.inl.hpp"

