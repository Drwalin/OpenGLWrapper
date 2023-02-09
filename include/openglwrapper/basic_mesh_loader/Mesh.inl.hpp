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

#ifndef OPEN_GL_BASIC_MESH_LOADER_MESH_INL_HPP
#define OPEN_GL_BASIC_MESH_LOADER_MESH_INL_HPP

#include "Mesh.hpp"

#include <limits>
#include <cmath>

namespace gl {
namespace BasicMeshLoader {
	template<typename T>
	void Mesh::AppendIndices(
			uint32_t vertexBaseOffsetWithData,
			std::vector<uint8_t>& elementBuffer
			) const {
		uint32_t offset = elementBuffer.size();
		elementBuffer.resize(offset + indices.size()*sizeof(T));
		T* inds = (T*)&(elementBuffer[offset]);
		for(int32_t i=0; i<indices.size(); ++i) {
			inds[i] = indices[i] + vertexBaseOffsetWithData;
		}
	}
	
	template<typename T>
	void Mesh::ExtractPos(
			uint32_t baseOffset, // bytes
			std::vector<uint8_t>& buffer,
			uint32_t offset,
			uint32_t stride,
			void(*converter)(T* dst, Value<3> value)
			) const {
		Copy(
				baseOffset,
				buffer,
				offset,
				stride,
				pos,
				converter);
	}
	

	template<typename T>
	void Mesh::ExtractNormal(
			uint32_t baseOffset, // bytes
			std::vector<uint8_t>& buffer,
			uint32_t offset,
			uint32_t stride,
			void(*converter)(T* dst, Value<3> value)
			) const {
		Copy(
				baseOffset,
				buffer,
				offset,
				stride,
				normal,
				converter);
	}
			
	template<typename T>
	void Mesh::ExtractUV(
			uint32_t baseOffset,
			std::vector<uint8_t>& buffer,
			uint32_t offset,
			uint32_t stride,
			void(*converter)(T* dst, Value<2> value),
			uint32_t channelId,
			uint32_t subsequentChannels 
			) const {
		if(uv.size() <= channelId+subsequentChannels) {
			// only informational warning
		}
		for(uint32_t i=0; i<subsequentChannels; ++i) {
			if(i+channelId >= uv.size())
				break;
			Copy(
					baseOffset,
					buffer,
					offset+sizeof(T)*2*i,
					stride,
					uv[i+channelId],
					converter);
		}
	}
			
	template<typename T>
	void Mesh::ExtractColor(
			uint32_t baseOffset,
			std::vector<uint8_t>& buffer,
			uint32_t offset,
			uint32_t stride,
			void(*converter)(T* dst, Value<4> value),
			uint32_t channelId,
			uint32_t subsequentChannels
			) const {
		if(color.size() <= channelId+subsequentChannels) {
			// only informational warning
		}
		for(uint32_t i=0; i<subsequentChannels; ++i) {
			if(i+channelId >= color.size())
				break;
			Copy(
					baseOffset,
					buffer,
					offset+sizeof(T)*4*i,
					stride,
					color[i+channelId],
					converter);
		}
	}
			
	template<typename Tweight, typename Tbone>
	void Mesh::ExtractWeightsWithBones(
			uint32_t baseOffset,
			std::vector<uint8_t>& buffer,
			uint32_t weightsOffset,
			uint32_t boneIdsOffset,
			uint32_t stride,
			void(*converterWeight)(Tweight* dst, Value<1> value),
			uint32_t weightsCount
			) const {
		std::vector<VertexBoneWeight> w;
		for(uint32_t i=0; i<weight.size(); ++i) {
			w = weight[i];
			if(w.size() > weightsCount)
				w.resize(weightsCount);
			if(w.size() < weightsCount)
				w.resize(weightsCount, VertexBoneWeight(0, 0));
			float sum = 0;
			for(auto v : w)
				sum += v.weight;
			if(sum > 0)
				for(auto v : w)
					v.weight /= sum;
			else
				for(auto v : w)
					v.weight = 0;
// 			printf("bone ids: ");
			for(int j=0; j<w.size(); ++j) {
				Tweight* wb = (Tweight*)&(buffer[baseOffset + i*stride + weightsOffset + sizeof(Tweight)*j]);
				converterWeight(wb, {w[j].weight});
				Tbone* bb = (Tbone*)&(buffer[baseOffset + i*stride + boneIdsOffset + sizeof(Tbone)*j]);
				bb[0] = w[j].boneId;
// 				printf(" %i -> %i (%f)\n", bb[0], wb[0], w[j].weight);
			}
// 			printf("\n");
		}
	}
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

