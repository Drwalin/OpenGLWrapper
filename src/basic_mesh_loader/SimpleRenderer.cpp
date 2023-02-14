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

#include "../../include/openglwrapper/basic_mesh_loader/SimpleRender.hpp"

#include "../../include/openglwrapper/basic_mesh_loader/AssimpLoader.hpp"

namespace gl {
namespace BasicMeshLoader {
	StaticMeshRenderable::StaticMeshRenderable(std::string fileModelName,
			uint32_t modelId,
			std::shared_ptr<Shader> shader,
			std::string positionName,
			std::string uvName,
			std::string colorName,
			std::string normalName) {
		gl::BasicMeshLoader::AssimpLoader l;
		l.Load(fileModelName);
		std::shared_ptr<gl::BasicMeshLoader::Mesh> mesh;
		if(l.meshes.size() > modelId) {
			mesh = l.meshes[modelId];
			Init(mesh, shader, positionName, uvName, colorName, normalName);
		} else {
			// TODO: print error
		}
	}
	
	StaticMeshRenderable::StaticMeshRenderable(std::shared_ptr<Mesh> mesh,
			std::shared_ptr<Shader> shader,
			std::string positionName,
			std::string uvName,
			std::string colorName,
			std::string normalName) {
		Init(mesh, shader, positionName, uvName, colorName, normalName);
	}

	void StaticMeshRenderable::Init(std::shared_ptr<Mesh> mesh,
			std::shared_ptr<Shader> shader,
			std::string positionName,
			std::string uvName,
			std::string colorName,
			std::string normalName) {
		this->shader = shader;
		
		uint32_t stride
			= 3*sizeof(float)
			+ 2*sizeof(float)
			+ 4*sizeof(uint8_t)
			+ 4*sizeof(uint8_t);
		vbo = std::make_shared<VBO>(stride, gl::ARRAY_BUFFER, gl::STATIC_DRAW);
		elements = std::make_shared<VBO>(4, gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
		
		mesh->ExtractPos<float>(0, vbo->Buffer(), 0, stride,
				gl::BasicMeshLoader::ConverterFloatPlain<float, 3>);
		
		mesh->ExtractUV<float>(0, vbo->Buffer(), 12, stride,
				gl::BasicMeshLoader::ConverterFloatPlain<float, 2>);
		
		mesh->ExtractColor<uint8_t>(0, vbo->Buffer(), 20, stride,
				gl::BasicMeshLoader::ConverterIntPlainClampScale<uint8_t, 255, 0, 255, 4>);
		
		mesh->ExtractNormal(0, vbo->Buffer(), 24, stride,
				gl::BasicMeshLoader::ConverterIntNormalized<uint8_t, 127, 3>);
		
		vbo->Generate();
		
		mesh->AppendIndices<uint32_t>(0, elements->Buffer());
		elements->Generate();
		
		
		vao = std::make_shared<VAO>(gl::TRIANGLES);
		vao->SetAttribPointer(*vbo, shader->GetAttributeLocation(positionName), 3, gl::FLOAT, false, 0);
		vao->SetAttribPointer(*vbo, shader->GetAttributeLocation(uvName), 2, gl::FLOAT, false, 12);
		vao->SetAttribPointer(*vbo, shader->GetAttributeLocation(colorName), 4, gl::UNSIGNED_BYTE, true, 20);
		vao->SetAttribPointer(*vbo, shader->GetAttributeLocation(normalName), 3, gl::BYTE, true, 24);
		vao->BindElementBuffer(*elements, gl::UNSIGNED_INT);
	}
	
	void StaticMeshRenderable::Draw() {
		if(shader && vao) {
			shader->Use();
			vao->Draw();
		}
	}
	
} // namespace BasicMeshLoader
} // namespace gl


