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

#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include <memory>
#include <string>
#include <map>
#include <iostream>

#include "VBO.hpp"
#include "VAO.hpp"
#include "OpenGL.hpp"
#include "Mesh.hpp"

namespace gl {
	
	class MeshManager {
	public:
		
		MeshManager();
		~MeshManager();
		
// 		void Save(std::ostream& out);
// 		void Load(std::istream& in);
		
// 		std::shared_ptr<Mesh> MergeMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes);
		
		std::shared_ptr<Mesh> CreateMeshFrom(std::shared_ptr<BasicMeshLoader::Mesh> basicMesh);
		
		void RemoveMesh(std::shared_ptr<Mesh> mesh);
		
		uint32_t AllocateVertices(uint32_t count);
		uint32_t AllocateElements(uint32_t count);
		
		friend class Mesh;
		
	private:
		
		void FreeVertices(uint32_t start, uint32_t count);
		void FreeElements(uint32_t start, uint32_t count);
		
		VBO vbo, elements;
		VAO vao;
		
		std::map<std::string, std::shared_ptr<Mesh>> meshes;
		std::map<uint32_t, std::shared_ptr<Mesh>> startingPointsOfVertexMeshes;
		std::map<uint32_t, std::shared_ptr<Mesh>> startingPointsOfElementsMeshes;
		std::map<uint32_t, uint32_t> startingPointsOfFreeSpaceWithItsSize;
	};
}

#endif

