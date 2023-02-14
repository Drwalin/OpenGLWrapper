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

#ifndef MESH_H
#define MESH_H

#include <memory>
#include <string>

#include "VBO.hpp"
#include "VAO.hpp"
#include "OpenGL.hpp"

namespace gl {
	
	class Skeleton;
	class MeshManager;
	
	namespace BasicMeshLoader {
		class Mesh;
	}	

	class Mesh {
	public:
		
		Mesh(std::weak_ptr<MeshManager> manager);
		Mesh(std::weak_ptr<MeshManager> manager, std::shared_ptr<BasicMeshLoader::Mesh> basciMesh);
		~Mesh();
		
		void Init(std::weak_ptr<MeshManager> manager);
		
		void RemoveFromManager();
		
		void Draw();
		
	public:
		
		inline uint32_t GetElementIndexStart() const { return start_element; }
		inline uint32_t GetElementsCount() const { return count_elements; }
		
	private:
		
		std::string name;
		
		std::weak_ptr<MeshManager> meshManager;
		std::shared_ptr<Skeleton> skeleton;
		
		uint32_t start_element, count_elements;
		uint32_t start_vertex, count_vertices;
	};
}

#endif

