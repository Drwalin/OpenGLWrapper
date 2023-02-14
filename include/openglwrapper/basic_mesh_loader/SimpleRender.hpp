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

#ifndef OPEN_GL_BASIC_MESH_LOADER_SIMPLE_RENDER_HPP
#define OPEN_GL_BASIC_MESH_LOADER_SIMPLE_RENDER_HPP

#include "Mesh.hpp"
#include "../VAO.hpp"
#include "../VBO.hpp"
#include "../Shader.hpp"

namespace gl {
namespace BasicMeshLoader {

	class StaticMeshRenderable {
	public:
		
		
		StaticMeshRenderable(std::string fileModelName,
				std::shared_ptr<Shader> shader,
				std::string positionName,
				std::string uvName,
				std::string colorName,
				std::string normalName);
		
		StaticMeshRenderable(std::shared_ptr<Mesh> mesh,
				std::shared_ptr<Shader> shader,
				std::string positionName,
				std::string uvName,
				std::string colorName,
				std::string normalName);
		
		void Init(std::shared_ptr<Mesh> mesh,
				std::shared_ptr<Shader> shader,
				std::string positionName,
				std::string uvName,
				std::string colorName,
				std::string normalName);
		
		void Draw();
		
		std::shared_ptr<Shader> shader;
		std::shared_ptr<VBO> vbo, elements;
		std::shared_ptr<VAO> vao;
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

