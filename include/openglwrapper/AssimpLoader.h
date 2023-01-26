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

#ifndef OPEN_GL_ASSIMP_LOADER_H
#define OPEN_GL_ASSIMP_LOADER_H

#include <cinttypes>
#include <cmath>

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <string>

#include <glm/glm.hpp>

class aiScene;
class aiMesh;

namespace gl {
	class AssimpLoader {
	public:
		
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
		
		std::vector<Mesh> meshes;
		
		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> norm;
		std::vector<glm::vec4> color;
		std::vector<glm::vec2> uv;
		
		std::vector<uint32_t> indices;
		
		void Load(const char* file);
		
		
		
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
	};
	
	
	
	
	
	template<typename T>
	void AssimpLoader::Mesh::AppendIndices(
			uint32_t vertexBaseOffsetWithData,
			std::vector<uint8_t>& elementBuffer
			) {
		uint32_t offset = elementBuffer.size();
		elementBuffer.resize(offset + indices.size()*sizeof(T));
		T* inds = (T*)&(indices[offset]);
		for(int32_t i=0; i<indices.size(); ++i, ++inds) {
			*inds = vertexBaseOffsetWithData+indices[i];
		}
	}
	
	template<typename T, bool normalize, uint32_t dim>
	void AssimpLoader::Mesh::ExtractAttribute(
			std::vector<std::vector<Value<dim>>> Mesh::* attribute,
			uint32_t baseOffset,
			std::vector<uint8_t>& buffer,
			uint32_t offset,
			uint32_t stride,
			uint32_t channelId,
			uint32_t subsequentChannels
			) const {
		
		if(buffer.size() < baseOffset + pos.size()*stride)
			buffer.resize(baseOffset + pos.size()*stride);
		for(uint32_t i=0; i<this->*attribute.size(); ++i) {
			for(uint32_t j=0; j<subsequentChannels; ++j) {
				T* dst = (T*)(buffer[baseOffset + i*stride + offset + j*dim*sizeof(T)]);
				Convert<T>(dst, this->*attribute[channelId+j][i], normalize);
			}
		}
	}
	
	template<uint32_t dim>
	constexpr inline uint32_t AssimpLoader::GetBasicElementsOfAttribute(std::vector<std::vector<AssimpLoader::Value<dim>>> Mesh::* attribute) {
		return dim;
	}
	
	template<typename T, bool normalize, bool negative, uint32_t dim>
	inline void AssimpLoader::Convert(T* dst, AssimpLoader::Value<dim> value) {
		if constexpr (normalize && dim > 1) {
			float sum = 0;
			for(int i=0; i<dim; ++i) {
				sum += value.v[i];
			}
			sum = sqrt(sum);
			for(int i=0; i<dim; ++i) {
				value.v[i] /= sum;
			}
		}
		
		if constexpr (std::is_same<T, float>() || std::is_same<T, double>() || std::is_same<T, long double>()) {
			for(int i=0; i<dim; ++i) {
				dst[i] = value.v[i];
			}
		} else {
			if constexpr ((((T)(0))-((T)(1)))  < 0) {
				if constexpr (normalize) {
					const T max = (((T)1)<<(sizeof(T)*8-1))-(T)1;
					for(int i=0; i<dim; ++i) {
						dst[i] = (value.v[i]*((float)max))+0.4999999f;
					}
				} else {
					for(int i=0; i<dim; ++i) {
						dst[i] = value.v[i]+0.4999999f;
					}
				}
			} else {
				if constexpr (normalize) {
					for(int i=0; i<dim; ++i) {
						dst[i] = (value.v[i]*((float)(((T)(0))-((T)(1)))))+0.4999999f;
					}
				} else {
					for(int i=0; i<dim; ++i) {
						dst[i] = value.v[i]+0.4999999f;
					}
				}
			}
		}
	}
	
} // namespace gl

#endif

