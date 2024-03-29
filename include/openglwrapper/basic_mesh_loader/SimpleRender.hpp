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

#ifndef OGLW_BASIC_MESH_LOADER_SIMPLE_RENDER_HPP
#define OGLW_BASIC_MESH_LOADER_SIMPLE_RENDER_HPP

#include "Mesh.hpp"
#include "../VAO.hpp"
#include "../VBO.hpp"
#include "../Shader.hpp"

namespace gl {
namespace BasicMeshLoader {

	class StaticMeshRenderable {
	public:
		
		StaticMeshRenderable(std::string fileModelName,
				uint32_t modelId,
				std::shared_ptr<Shader> shader,
				std::string positionName = "pos",
				std::string uvName = "uv",
				std::string colorName = "color",
				std::string normalName = "normal");
		
		StaticMeshRenderable(std::string fileModelName,
				std::shared_ptr<Shader> shader,
				std::string positionName = "pos",
				std::string uvName = "uv",
				std::string colorName = "color",
				std::string normalName = "normal");
		
		StaticMeshRenderable(std::shared_ptr<Mesh> mesh,
				std::shared_ptr<Shader> shader,
				std::string positionName = "pos",
				std::string uvName = "uv",
				std::string colorName = "color",
				std::string normalName = "normal");
		
		void Init(std::shared_ptr<Mesh> mesh,
				std::shared_ptr<Shader> shader,
				std::string positionName = "pos",
				std::string uvName = "uv",
				std::string colorName = "color",
				std::string normalName = "normal");
		
		void Draw();
		
		std::shared_ptr<Shader> shader;
		std::shared_ptr<VBO> vbo, elements;
		std::shared_ptr<VAO> vao;
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

