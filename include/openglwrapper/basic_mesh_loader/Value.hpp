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

#ifndef OPEN_GL_BASIC_MESH_LOADER_VALUE_HPP
#define OPEN_GL_BASIC_MESH_LOADER_VALUE_HPP

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
	
	
	
	template<typename T, uint32_t dim>
	inline void Copy(
			uint32_t dstOffset,
			std::vector<uint8_t>& dst,
			uint32_t offset,
			uint32_t stride,
			const std::vector<Value<dim>>& src,
			void(*converter)(T* dst, Value<dim> value));
	
	
	
	template<typename T, uint32_t dim>
	void ConverterFloatPlain(T* dst, Value<dim> value);
	
	template<typename T, uint32_t dim>
	void ConverterFloatNormalized(T* dst, Value<dim> value);
	
	template<typename T, uint32_t dim>
	void ConverterIntPlain(T* dst, Value<dim> value);
	
	template<typename T, T min, T max, uint32_t dim>
	void ConverterIntPlainClamp(T* dst, Value<dim> value);
	
	template<typename T, T scale, T min, T max, uint32_t dim>
	void ConverterIntPlainClampScale(T* dst, Value<dim> value);
	
	template<typename T, T len, uint32_t dim>
	void ConverterIntNormalized(T* dst, Value<dim> value);
	
	template<typename T, T mid, T max_len, uint32_t dim>
	void ConverterUnsignedNormalized(T* dst, Value<dim> value);
	
	
	
	template<uint32_t dim>
	inline Value<dim> Normalize(Value<dim> value);
	
	
	
	
	
	
	template<typename T, uint32_t dim>
	inline void Copy(
			uint32_t dstOffset,
			std::vector<uint8_t>& dst,
			uint32_t offset,
			uint32_t stride,
			const std::vector<Value<dim>>& src,
			void(*converter)(T* dst, Value<dim> value)
			) {
		
		if(dst.size() < dstOffset + src.size()*stride)
			dst.resize(dstOffset + src.size()*stride);
		for(uint32_t i=0; i<src.size(); ++i) {
			T* _dst = (T*)&(dst[dstOffset + i*stride + offset]);
			converter(_dst, src[i]);
		}
	}
	
	
	
	template<typename T, uint32_t dim>
	void ConverterFloatPlain(T* dst, Value<dim> value) {
		for(int i=0; i<dim; ++i)
			dst[i] = value.v[i];
	}
	
	template<typename T, uint32_t dim>
	void ConverterFloatNormalized(T* dst, Value<dim> value) {
		ConvertFloatPlain(dst, Normalize(value));
	}
	
	template<typename T, uint32_t dim>
	void ConverterIntPlain(T* dst, Value<dim> value) {
		for(int i=0; i<dim; ++i)
			dst[i] = value.v[i]+0.5f;
	}
	
	template<typename T, T min, T max, uint32_t dim>
	void ConverterIntPlainClamp(T* dst, Value<dim> value) {
		for(int i=0; i<dim; ++i) {
			float v = value.v[i]+0.5f;
			if(v <= min)
				dst[i] = min;
			else if(v >= max)
				dst[i] = max;
			else
				dst[i] = v;
		}
	}
	
	template<typename T, T scale, T min, T max, uint32_t dim>
	void ConverterIntPlainClampScale(T* dst, Value<dim> value) {
		for(int i=0; i<dim; ++i) {
			float v = (value.v[i]*(float)scale)+0.5f;
			if(v <= (float)min)
				dst[i] = min;
			else if(v >= (float)max)
				dst[i] = max;
			else
				dst[i] = v;
		}
	}
	
	template<typename T, T max, uint32_t dim>
	void ConverterIntNormalized(T* dst, Value<dim> value) {
		value = Normalize(value);
		for(int i=0; i<dim; ++i)
			dst[i] = ((float)max)*(value.v[i]);
	}
	
	template<typename T, T mid, T max_len, uint32_t dim>
	void ConverterUnsignedNormalized(T* dst, Value<dim> value) {
		value = Normalize(value);
		for(int i=0; i<dim; ++i)
			dst[i] = (((float)max_len)*(value.v[i]))+mid;
	}
	
	
	
	
	
	
		
	template<uint32_t dim>
	inline Value<dim> Normalize(Value<dim> value) {
		if constexpr (dim > 1) {
			float sum = 0;
			for(int i=0; i<dim; ++i) {
				sum += value.v[i]*value.v[i];
			}
			sum = sqrt(sum);
			for(int i=0; i<dim; ++i) {
				value.v[i] /= sum;
			}
		} else {
			if(value.v[0] < -1)
				return {-1};
			else if(value.v[0] > 1)
				return {1};
		}
		return value;
	}
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

