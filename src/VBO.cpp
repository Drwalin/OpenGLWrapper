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

#include "../include/openglwrapper/VBO.hpp"

#include <cstdio>

namespace gl {

VBO::VBO(uint32_t vertexSize, gl::BufferTarget target, gl::BufferUsage usage) :
		target(target), usage(usage), vertexSize(vertexSize) {
	this->vertexSize = vertexSize;
	this->target = target;
	this->usage = usage;
	vboID = 0;
	vertices = 0;
	immutable = false;
}

VBO::~VBO() {
	Destroy();
}

void VBO::InitImmutable(const void* data, uint32_t vertexCount,
		GLbitfield flags) {
	if(vboID) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot initialize object that is already initialized.");
		return;
	}
	vertices = vertexCount;
	GL_CHECK_PUSH_ERROR;
	glCreateBuffers(1, &vboID);
	GL_CHECK_PUSH_ERROR;
	glNamedBufferStorage(vboID, vertexSize*vertices, data, flags);
	GL_CHECK_PUSH_ERROR;
	immutable = true;
}

void VBO::Init() {
	if(vboID) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot initialize object that is already initialized.");
		return;
	}
	GL_CHECK_PUSH_ERROR;
	glCreateBuffers(1, &vboID);
	GL_CHECK_PUSH_ERROR;
	glNamedBufferData(vboID, vertexSize, nullptr, usage);
	GL_CHECK_PUSH_ERROR;
	Generate(nullptr, 1);
	vertices = 1;
	GL_CHECK_PUSH_ERROR;
	immutable = false;
}

void VBO::Destroy() {
	if(vboID) {
		glDeleteBuffers(1, &vboID);
		GL_CHECK_PUSH_ERROR;
		vboID = 0;
		immutable = false;
	}
}

void VBO::Generate(const void* data, uint32_t vertexCount) {
	GL_CHECK_PUSH_ERROR;
	if(immutable) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot call VBO::Generate on immutable object.");
		return;
	}
	if(!vboID) {
		Init();
	}
	GL_CHECK_PUSH_ERROR;
	vertices = vertexCount;
	glNamedBufferData(vboID, vertexSize*vertexCount, data, usage);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Generate(const std::vector<uint8_t>& data) {
	Generate(&data.front(), (data.size()+vertexSize-1)/vertexSize);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Update(const void* data, uint32_t offset, uint32_t bytes) {
	GL_CHECK_PUSH_ERROR;
	if(!vboID) {
		Init();
	}
	GL_CHECK_PUSH_ERROR;
	if(vertexSize*vertices < offset+bytes) {
		if(immutable) {
			GL_PUSH_CUSTOM_ERROR(999999999, "Cannot resize immutable VBO object.");
			return;
		}
		Resize((offset+bytes+vertexSize-1)/vertexSize);
	}
	GL_CHECK_PUSH_ERROR;
	glNamedBufferSubData(vboID, offset, bytes, data);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Fetch(void* data, uint32_t offset, uint32_t bytes) {
	if(vboID) {
		if(offset+bytes > vertexSize*vertices) {
			if(offset >= vertexSize*vertices) {
				return;
			}
			bytes = vertexSize*vertices - offset;
		}
		glGetNamedBufferSubData(vboID, offset, bytes, data);
		GL_CHECK_PUSH_ERROR;
	}
}

void VBO::FetchAll(std::vector<uint8_t>& data) {
	data.resize(vertices*vertexSize);
	Fetch(data.data(), 0, data.size());
	GL_CHECK_PUSH_ERROR;
}

void VBO::BindBufferBase(gl::BufferTarget target, int location) {
	GL_CHECK_PUSH_ERROR;
	if(!vboID) {
		Init();
	}
	GL_CHECK_PUSH_ERROR;
	glBindBufferBase(target, location, vboID);
	GL_CHECK_PUSH_ERROR;
}

void VBO::Resize(uint32_t newVertices) {
	if(immutable) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot resize immutable VBO object.");
		return;
	}
	if(vertices == newVertices) {
		return;
	}
	uint32_t toCopyBytes = std::min(newVertices, vertices)*vertexSize;
	std::vector<uint8_t> buffer;
	buffer.resize(toCopyBytes);
	Fetch(buffer.data(), 0, toCopyBytes);
	Generate(nullptr, newVertices);
	Update(&buffer.front(), 0, toCopyBytes);
}

void VBO::Copy(VBO* readBuffer, uint32_t readOffset, uint32_t writeOffset, uint32_t bytes) {
	if(readBuffer) {
		if(vboID && readBuffer->vboID) {
			glCopyNamedBufferSubData(readBuffer->vboID, vboID, readOffset, writeOffset, bytes);
			GL_CHECK_PUSH_ERROR;
		}
	}
}

} // namespace gl

