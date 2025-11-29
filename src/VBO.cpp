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

#include <mutex>
#include <set>

#include "../include/openglwrapper/VBO.hpp"

namespace gl {
	
static std::set<VBO*> allVbos;
static std::mutex mutex;

VBO::VBO(uint32_t vertexSize, gl::BufferTarget target, gl::BufferUsage usage) :
		target(target), usage(usage), vertexSize(vertexSize) {
	this->vertexSize = vertexSize;
	this->target = target;
	this->usage = usage;
	vboID = 0;
	vertices = 0;
	immutable = false;
	mappedPointer = nullptr;
	std::lock_guard<std::mutex> lock(mutex);
	allVbos.insert(this);
}

VBO::~VBO() {
	Destroy();
	std::lock_guard<std::mutex> lock(mutex);
	allVbos.erase(this);
}

void VBO::InitImmutable(const void* data, uint32_t vertexCount,
		GLbitfield flags) {
	if(vboID) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot initialize object that is already initialized.");
		return;
	}
	immutableFlags = flags;
	vertices = vertexCount;
	GL_CHECK_PUSH_ERROR;
	glCreateBuffers(1, &vboID);
	GL_CHECK_PUSH_ERROR;
	glNamedBufferStorage(vboID, vertexSize*vertices, data, immutableFlags);
	GL_CHECK_PUSH_ERROR;
	immutable = true;
}

void VBO::Init() {
	Init(1);
}

void VBO::Init(uint32_t vertexCount) {
	if(vboID) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot initialize object that is already initialized.");
		return;
	}
	GL_CHECK_PUSH_ERROR;
	glCreateBuffers(1, &vboID);
	GL_CHECK_PUSH_ERROR;
	if (vboID == 0) {
		return;
	}
	Generate(nullptr, vertexCount);
	immutable = false;
}

void VBO::Destroy() {
	if(vboID) {
		if(mappedPointer) {
			glUnmapNamedBuffer(vboID);
			mappedPointer = nullptr;
		}
		glDeleteBuffers(1, &vboID);
		GL_CHECK_PUSH_ERROR;
		vboID = 0;
		immutable = false;
		vertices = 0;
	}
}

void* VBO::InitMapPersistent(const void* data, uint32_t vertexCount,
		GLbitfield flags) {
	if(vboID) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot initialize object that is already initialized.");
		return nullptr;
	}
	InitImmutable(data, vertexCount,
			(flags | DYNAMIC_STORAGE_BIT | MAP_PERSISTENT_BIT)
				& (
					DYNAMIC_STORAGE_BIT |
					MAP_READ_BIT |
					MAP_WRITE_BIT |
					MAP_PERSISTENT_BIT |
					MAP_COHERENT_BIT |
					CLIENT_STORAGE_BIT
				)
			);
	mappedPointer = (uint32_t*)glMapNamedBufferRange(
			vboID, 0, vertexSize*vertices,
			(flags | MAP_PERSISTENT_BIT)
				& (
					MAP_READ_BIT |
					MAP_WRITE_BIT |
					MAP_PERSISTENT_BIT |
					MAP_COHERENT_BIT |
					MAP_INVALIDATE_BUFFER_BIT |
					MAP_INVALIDATE_RANGE_BIT |
					MAP_FLUSH_EXPLICIT_BIT |
					MAP_UNSYNCHRONIZED_BIT
				));
	GL_CHECK_PUSH_ERROR;
	return mappedPointer;
}

void* VBO::GetMappedPointer() {
	return mappedPointer;
}

void VBO::FlushToGpuMapPersistentFullRange() {
	glFlushMappedNamedBufferRange(vboID, 0, vertexSize*vertices);
	GL_CHECK_PUSH_ERROR;
}

void VBO::FlushToGpuMapPersistent(uint32_t offsetVertex, uint32_t verticesCount) {
	glFlushMappedNamedBufferRange(vboID, offsetVertex*vertexSize,
			verticesCount*vertexSize);
	GL_CHECK_PUSH_ERROR;
}

void VBO::FlushFromGpuMapPersistentFullRange() {
	gl::MemoryBarrier(gl::CLIENT_MAPPED_BUFFER_BARRIER_BIT);
}

void* VBO::ResizePersistentMapped(uint32_t newVertices) {
	throw "VBO::ResizePersistentMapped is undocumented and not tested.";
	std::vector<uint8_t> data;
	data.reserve(newVertices*vertexSize);
	data.resize(vertices*vertexSize);
	gl::MemoryBarrier(gl::ALL_BARRIER_BITS);
	gl::Finish();
	memcpy(data.data(), mappedPointer, data.size());
	data.resize(newVertices*vertexSize);
	GLbitfield immutableFlags = this->immutableFlags;
	GLbitfield mapFlags = this->mapFlags;
	Destroy();
	return InitMapPersistent(data.data(), newVertices, immutableFlags|mapFlags);
}



void VBO::Generate(const void* data, uint32_t vertexCount) {
	GL_CHECK_PUSH_ERROR;
	if(immutable) {
		GL_PUSH_CUSTOM_ERROR(999999999, "Cannot call VBO::Generate on immutable object.");
		return;
	}
	if(!vboID) {
		Init(vertexCount);
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
		Init((offset+bytes+vertexSize-1)/vertexSize);
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

uint64_t VBO::CountAllVBOMemoryUsage() {
	std::lock_guard<std::mutex> lock(mutex);
	uint64_t bytes = 0;
	for(auto v : allVbos) {
		if(v->GetIdGL()) {
			bytes += (uint64_t)v->GetVertexCount() * (uint64_t)v->VertexSize();
		}
	}
	return bytes;
}

} // namespace gl

