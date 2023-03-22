/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021-2023 Marek Zalewski aka Drwalin
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

#include <cstdio>

#include "../include/openglwrapper/VBO.hpp"

namespace gl {

VBO::VBO(uint32_t vertexSize, gl::BufferTarget target, gl::BufferUsage usage) :
		target(target), usage(usage), vertexSize(vertexSize) {
	vboID = 0;
	this->vertexSize = vertexSize;
	this->target = target;
	this->usage = usage;
	this->vertices = 0;
}

VBO::~VBO() {
	Destroy();
}

void VBO::Init() {
	if(vboID == 0) {
		glCreateBuffers(1, &vboID);
		glNamedBufferData(vboID, vertexSize, NULL, usage);
		GL_CHECK_PUSH_ERROR;
		Generate(nullptr, 1);
		vertices = 1;
	}
}

void VBO::Destroy() {
	if(vboID) {
		glDeleteBuffers(1, &vboID);
		vboID = 0;
		GL_CHECK_PUSH_ERROR;
	}
}

void VBO::Generate(const void* data, uint32_t vertexCount) {
	Init();
	vertices = vertexCount;
	glNamedBufferData(vboID, vertexSize*vertexCount, data, usage);
	printf(" vbo generate(%i)\n", vertices);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Generate(const std::vector<uint8_t>& data) {
	Generate(&data.front(), (data.size()+vertexSize-1)/vertexSize);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Update(const void* data, uint32_t offset, uint32_t bytes) {
	Init();
	if(vertexSize*vertices < offset+bytes) {
		Resize((offset+bytes+vertexSize-1)/vertexSize);
	}
	glNamedBufferSubData(vboID, offset, bytes, data);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Fetch(void* data, uint32_t offset, uint32_t bytes) {
	if(vboID) {
		glGetNamedBufferSubData(vboID, offset, bytes, data);
		GL_CHECK_PUSH_ERROR;
	}
}

void VBO::FetchAll(std::vector<uint8_t>& data) {
	data.resize(vertices*vertexSize);
	Fetch(&data.front(), 0, data.size());
	GL_CHECK_PUSH_ERROR;
}

void VBO::BindBufferBase(gl::BufferTarget target, int location) {
	Init();
	glBindBufferBase(target, location, vboID);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Resize(uint32_t newVertices) {
	if(vertices == newVertices) {
		return;
	}
	GL_CHECK_PUSH_ERROR;
	Init();
	GL_CHECK_PUSH_ERROR;
	VBO temp(vertexSize, target, usage);
	GL_CHECK_PUSH_ERROR;
	if(newVertices < vertices) {
		temp.Generate(NULL, newVertices);
	GL_CHECK_PUSH_ERROR;
		temp.Copy(this, 0, 0, newVertices*vertexSize);
	GL_CHECK_PUSH_ERROR;
	} else if(newVertices > vertices) {
		temp.Generate(NULL, vertices);
	GL_CHECK_PUSH_ERROR;
		temp.Copy(this, 0, 0, vertices*vertexSize);
	GL_CHECK_PUSH_ERROR;
	}
	GL_CHECK_PUSH_ERROR;
	Generate(NULL, newVertices*vertexSize);
	GL_CHECK_PUSH_ERROR;
	this->Copy(&temp, 0, 0, std::min(vertices, newVertices) * vertexSize);
	GL_CHECK_PUSH_ERROR;
	printf(" vbo.resize(%u -> %u)\n", vertices, newVertices);
	vertices = newVertices;
}

void VBO::Copy(VBO* readBuffer, uint32_t readOffset, uint32_t writeOffset, uint32_t bytes) {
	if(readBuffer) {
		if(vboID && readBuffer->vboID) {
			glCopyNamedBufferSubData(readBuffer->vboID, vboID, readOffset, writeOffset, bytes);
		}
	}
}

} // namespace gl

