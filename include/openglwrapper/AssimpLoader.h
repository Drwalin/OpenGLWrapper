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

#include <vector>

#include <glm/glm.hpp>

namespace gl {
	class AssimpLoader {
	public:
		
		std::vector<glm::vec3> pos;
		std::vector<glm::vec3> norm;
		std::vector<glm::vec4> color;
		std::vector<glm::vec2> uv;
		std::vector<uint32_t> indices;
		
		void Load(const char* file);
	};
} // namespace gl

#endif

