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
			) {
		uint32_t offset = elementBuffer.size();
		elementBuffer.resize(offset + indices.size()*sizeof(T));
		T* inds = (T*)&(indices[offset]);
		for(int32_t i=0; i<indices.size(); ++i, ++inds) {
			*inds = vertexBaseOffsetWithData+indices[i];
		}
	}
} // namespace BasicMeshLoader
} // namespace gl

#endif

