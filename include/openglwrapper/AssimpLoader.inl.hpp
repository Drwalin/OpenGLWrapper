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

#ifndef OPEN_GL_ASSIMP_LOADER_INL_HPP
#define OPEN_GL_ASSIMP_LOADER_INL_HPP

#include <limits>
#include <cmath>

#include "AssimpLoader.hpp"

namespace gl {

	template<typename T, uint32_t dim>
	inline void AssimpLoader::Copy(
			uint32_t dstOffset,
			std::vector<uint8_t>& dst,
			uint32_t offset,
			uint32_t stride,
			const std::vector<Value<dim>>& src,
			void(*converter)(T* dst, AssimpLoader::Value<dim> value)
			) {
		
		if(dst.size() < dstOffset + src.size()*stride)
			dst.resize(dstOffset + src.size()*stride);
		for(uint32_t i=0; i<src.size(); ++i) {
			T* dst = (T*)(dst[dstOffset + i*stride + offset]);
			converter(dst, src[i]);
		}
	}
	
	
	
	template<typename T, uint32_t dim>
	void AssimpLoader::ConverterFloatPlain(T* dst, AssimpLoader::Value<dim> value) {
		for(int i=0; i<dim; ++i)
			dst[i] = value.v[i];
	}
	
	template<typename T, uint32_t dim>
	void AssimpLoader::ConverterFloatNormalized(T* dst, AssimpLoader::Value<dim> value) {
		ConvertFloatPlain(dst, Normalize(value));
	}
	
	template<typename T, uint32_t dim>
	void AssimpLoader::ConverterIntPlain(T* dst, AssimpLoader::Value<dim> value) {
		for(int i=0; i<dim; ++i)
			dst[i] = value.v[i]+0.5f;
	}
	
	template<typename T, T min, T max, uint32_t dim>
	void AssimpLoader::ConverterIntPlainClamp(T* dst, AssimpLoader::Value<dim> value) {
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
	
	template<typename T, T max, uint32_t dim>
	void AssimpLoader::ConverterIntNormalized(T* dst, AssimpLoader::Value<dim> value) {
		value = Normalize(value);
		for(int i=0; i<dim; ++i)
			dst[i] = ((float)max)*(value.v[i]);
	}
	
	template<typename T, T mid, T max_len, uint32_t dim>
	void AssimpLoader::ConverterUnsignedNormalized(T* dst, AssimpLoader::Value<dim> value) {
		value = Normalize(value);
		for(int i=0; i<dim; ++i)
			dst[i] = (((float)max_len)*(value.v[i]))+mid;
	}
	
	
	
	
	
	
		
	template<uint32_t dim>
	inline AssimpLoader::Value<dim> AssimpLoader::Normalize(AssimpLoader::Value<dim> value) {
		if constexpr (dim > 1) {
			float sum = 0;
			for(int i=0; i<dim; ++i) {
				sum += value.v[i];
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

