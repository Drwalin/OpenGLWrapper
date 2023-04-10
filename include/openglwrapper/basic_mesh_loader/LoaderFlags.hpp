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

#ifndef OGLW_BASIC_MESH_LOADER_LOADER_FLAGS_HPP
#define OGLW_BASIC_MESH_LOADER_LOADER_FLAGS_HPP

#include <cinttypes>

namespace gl {
namespace BasicMeshLoader {
	
	enum LoadingFlags : uint64_t {
		NONE = 0,
		RENAME_MESH_BY_FIRST_NODE_WITH_ITS_NAME = 1,
		CORRECT_NOT_ANIMATED_MESH_ORIENTATION = 2,
		CORRECT_ANIMATED_MESH_ORIENTATION_INSIDE_ROOT_BONE_TRANSFORMATION = 4,
	};
	
	using LoaderFlagsBitfield = uint64_t;
}
}

#endif

