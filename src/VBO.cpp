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

VBO::VBO(unsigned vertexSize, gl::BufferTarget target, gl::BufferUsage usage) :
	target(target), usage(usage), vertexSize(vertexSize) {
	vboID = 0;
	glCreateBuffers(1, &vboID);
}

VBO::~VBO() {
	if(vboID)
		glDeleteBuffers(1, &vboID);
}

void VBO::Generate() {
	Generate(&buffer.front(), buffer.size()/vertexSize);
}

void VBO::Generate(const void* data, uint32_t vertexCount) {
	vertices = buffer.size()/vertexSize;
	glNamedBufferData(vboID, vertices*vertexSize, data, usage);
}

void VBO::Update(unsigned beg, unsigned end) {
	end = std::min<unsigned>(end, vertices);
	if(beg >= end)
		return;
	unsigned offset = beg * vertexSize;
	unsigned size = (end - beg) * vertexSize;
	if(buffer.size() < offset+size) {
		printf("VBO::Update Return: beg=%u, end=%u, vertexSize=%u, offset=%u, size=%u, vertices=%u, buffer.size()=%lu\n",
				beg, end, vertexSize, offset, size, vertices, buffer.size());
		return;
	}
	glBindVertexArray(0);
	glBindBuffer(target, vboID);
	glBufferSubData(target, offset, size, &(buffer[offset]));
	glBindBuffer(target, 0);
	GL_CHECK_PUSH_PRINT_ERROR;
}

void VBO::SetType(unsigned vertexSize, gl::BufferTarget target,
		gl::BufferUsage usage) {
	this->vertexSize = vertexSize;
	this->target = target;
	this->usage = usage;
}

void VBO::ClearHostBuffer() {
	buffer.clear();
	buffer.shrink_to_fit();
}

void VBO::FetchAllDataToHostFromGPU() {
	buffer.resize(vertexSize*vertices);
	Fetch(&(buffer.front()), 0, vertexSize*vertices);
}

void VBO::Fetch(void* data, uint32_t offset, uint32_t bytes) {
	if(vboID) {
		glGetNamedBufferSubData(vboID, offset, bytes, data);
	}
}

void VBO::BindBufferBase(gl::BufferTarget target, int location) {
	glBindBufferBase(target, location, vboID);
}

void VBO::Resize(uint32_t newVertices) {
	VBO temp(vertexSize, target, usage);
	temp.Generate(NULL, vertices);
	temp.Copy(this, 0, 0, vertices*vertexSize);
	Generate(NULL, newVertices*vertexSize);
	this->Copy(&temp, 0, 0, vertices*vertexSize);
	vertices = newVertices;
}

void VBO::Copy(VBO* readBuffer, uint32_t readOffset, uint32_t writeOffset, uint32_t bytes) {
	if(readBuffer) {
		glCopyNamedBufferSubData(readBuffer->vboID, vboID, readOffset, writeOffset, bytes);
	}
}

} // namespace gl

