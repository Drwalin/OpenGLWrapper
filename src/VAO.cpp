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

#include "../include/openglwrapper/OpenGL.hpp"
#include "../include/openglwrapper/VBO.hpp"

#include "../include/openglwrapper/VAO.hpp"

namespace gl {
	
	unsigned VAO::currentVao = 0;

VAO::VAO(gl::VertexMode mode) : mode(mode) {
	sizeI = 0;
	sizeA = 0;
	vaoID = 0;
	drawArrays = true;
	instances = 0;
}

VAO::~VAO() {
}

void VAO::Init() {
	Unbind();
	glGenVertexArrays(1, &vaoID);
	Unbind();
	GL_CHECK_PUSH_ERROR;
}


void VAO::Delete() {
	glDeleteVertexArrays(1, &vaoID);
	GL_CHECK_PUSH_ERROR;
}

void VAO::SetAttribPointer(VBO& vbo, int location, unsigned count,
		gl::DataType type, bool normalized, unsigned offset, unsigned divisor) {
	GL_CHECK_PUSH_ERROR;
	if(!vbo.GetIdGL()) {
		vbo.Init();
	}
	GL_CHECK_PUSH_ERROR;
	Bind();
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(vbo.target, vbo.vboID);
	GL_CHECK_PUSH_ERROR;
	GL_CHECK_PUSH_ERROR;
	if(location >= 0) {
		glEnableVertexAttribArray(location);
		GL_CHECK_PUSH_ERROR;
		glVertexAttribPointer(location, count, type, normalized, vbo.vertexSize,
				(void*)(size_t)offset);
		GL_CHECK_PUSH_ERROR;
		glVertexAttribDivisor(location, divisor);
	}
	GL_CHECK_PUSH_ERROR;
	Unbind();
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(vbo.target, 0);
	GL_CHECK_PUSH_ERROR;
	if(divisor>0) {
		instances = std::max(instances, divisor*vbo.vertices);
	} else if(vbo.target == gl::ELEMENT_ARRAY_BUFFER || vbo.target == gl::DRAW_INDIRECT_BUFFER) {
		GL_PUSH_CUSTOM_ERROR(-666, "Cannot bind buffer of target GL_ELEMENT_ARRAY_BUFFER nor GL_DRAW_INDIRECT_BUFFER with VAO::SetAttribPointer");
	} else {
		sizeA = std::max(vbo.vertices, sizeA);
	}
}

void VAO::SetIntegerAttribPointer(VBO& vbo, int location, unsigned count,
		gl::DataType type, unsigned offset, unsigned divisor) {
	GL_CHECK_PUSH_ERROR;
	if(!vbo.GetIdGL()) {
		vbo.Init();
	}
	GL_CHECK_PUSH_ERROR;
	Bind();
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(vbo.target, vbo.vboID);
	GL_CHECK_PUSH_ERROR;
	GL_CHECK_PUSH_ERROR;
	if(location >= 0) {
		glEnableVertexAttribArray(location);
		GL_CHECK_PUSH_ERROR;
		glVertexAttribIPointer(location, count, type, vbo.vertexSize,
				(void*)(size_t)offset);
		GL_CHECK_PUSH_ERROR;
		glVertexAttribDivisor(location, divisor);
	}
	GL_CHECK_PUSH_ERROR;
	Unbind();
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(vbo.target, 0);
	GL_CHECK_PUSH_ERROR;
	if(divisor>0) {
		instances = std::max(instances, divisor*vbo.vertices);
	} else if(vbo.target == gl::ELEMENT_ARRAY_BUFFER || vbo.target == gl::DRAW_INDIRECT_BUFFER) {
		GL_PUSH_CUSTOM_ERROR(-666, "Cannot bind buffer of target GL_ELEMENT_ARRAY_BUFFER nor GL_DRAW_INDIRECT_BUFFER with VAO::SetAttribPointer");
	} else {
		sizeA = std::max(vbo.vertices, sizeA);
	}
}

void VAO::BindElementBuffer(VBO& ebo, gl::DataType type) {
	GL_CHECK_PUSH_ERROR;
	if(!ebo.GetIdGL()) {
		ebo.Init();
	}
	GL_CHECK_PUSH_ERROR;
	Bind();
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(gl::ELEMENT_ARRAY_BUFFER, ebo.vboID);
	GL_CHECK_PUSH_ERROR;
	Unbind();
	glBindBuffer(gl::ELEMENT_ARRAY_BUFFER, 0);
	drawArrays = false;
	sizeI = std::max(ebo.vertices, sizeI);
	typeElements = type;
}

void VAO::BindIndirectBuffer(VBO& indirectBO) {
	GL_CHECK_PUSH_ERROR;
	if(!indirectBO.GetIdGL()) {
		indirectBO.Init();
	}
	GL_CHECK_PUSH_ERROR;
	indirectDrawBuffer = &indirectBO;
}

void VAO::SetInstances(unsigned instances) {
	this->instances = instances;
}

void VAO::Draw() {
	Draw(0, drawArrays ? sizeA : sizeI);
}

void VAO::Draw(unsigned start, unsigned count) {
	if(drawArrays)
		DrawArrays(start, count);
	else
		DrawElements(start, count);
}

void VAO::DrawArrays(unsigned start, unsigned count) {
	Bind();
	if(instances)
		glDrawArraysInstanced(mode, start, count, instances);
	else
		glDrawArrays(mode, start, count);
}

void VAO::DrawElements(unsigned start, unsigned count) {
	Bind();
	void* offset = nullptr;
	switch(typeElements) {
		case gl::UNSIGNED_BYTE:
			offset = (void*)(size_t)(start*1);
			break;
		case gl::UNSIGNED_SHORT:
			offset = (void*)(size_t)(start*2);
			break;
		case gl::UNSIGNED_INT:
			offset = (void*)(size_t)(start*4);
			break;
		default:
			// TODO: error
			printf(" error in VAO::DrawElements: unusable element internal indexing type\n");
	}
	if(instances) {
		glDrawElementsInstanced(mode, count, typeElements, offset, instances);
	} else {
		glDrawElements(mode, count, typeElements, offset);
	}
}

void VAO::DrawMultiElementsIndirect(void* indirect, int drawCount,
		const int limitObjectDrawnPerSingleInvocation) {
	if(drawCount <= 0)
		return;
	if(indirectDrawBuffer == nullptr && indirect == nullptr) {
		GL_PUSH_CUSTOM_ERROR(-311, " error in VAO::DrawMultiElementsIndirect: "
				"indirect draw buffer is not bound to VAO nor there is no "
				"indirect host buffer passed\n");
		return;
	}
	Bind();
	GL_CHECK_PUSH_ERROR;
	switch(typeElements) {
		case gl::UNSIGNED_BYTE:
		case gl::UNSIGNED_SHORT:
		case gl::UNSIGNED_INT:
			break;
		default:
			// TODO: error
			printf(" error in VAO::DrawMultiElementsIndirect: unusable element internal indexing type\n");
	}
	if(indirectDrawBuffer)
		glBindBuffer(gl::DRAW_INDIRECT_BUFFER, indirectDrawBuffer->vboID);
	GL_CHECK_PUSH_ERROR;
	for(; drawCount>0;) {
		const int currentDrawCount
			= std::min(limitObjectDrawnPerSingleInvocation, drawCount);
		glMultiDrawElementsIndirect(mode, typeElements, indirect,
				currentDrawCount, 0);
		drawCount -= currentDrawCount;
		indirect = (void*)((size_t)indirect + currentDrawCount*20);
	}
	GL_CHECK_PUSH_ERROR;
}

void VAO::DrawMultiArraysIndirect(void* indirect, int drawCount,
		const int limitObjectDrawnPerSingleInvocation) {
	if(drawCount <= 0)
		return;
	if(indirectDrawBuffer == nullptr && indirect == nullptr) {
		GL_PUSH_CUSTOM_ERROR(-311, " error in VAO::DrawMultiArraysIndirect: "
				"indirect draw buffer is not bound to VAO nor there is no "
				"indirect host buffer passed\n");
	}
	Bind();
	GL_CHECK_PUSH_ERROR;
	if(indirectDrawBuffer)
		glBindBuffer(gl::DRAW_INDIRECT_BUFFER, indirectDrawBuffer->vboID);
	GL_CHECK_PUSH_ERROR;
	for(; drawCount>0;) {
		const int currentDrawCount = std::min(limitObjectDrawnPerSingleInvocation, drawCount);
		glMultiDrawArraysIndirect(mode, indirect, currentDrawCount, 0);
		drawCount -= currentDrawCount;
		indirect = (void*)((size_t)indirect + currentDrawCount*16);
	}
	GL_CHECK_PUSH_ERROR;
}

void VAO::SetSize(unsigned count) {
	if(drawArrays)
		sizeA = count;
	else
		sizeI = count;
}

void VAO::Bind() {
	if(currentVao != vaoID) {
		currentVao = vaoID;
		glBindVertexArray(vaoID);
		GL_CHECK_PUSH_ERROR;
	}
}

void VAO::Unbind() {
	if(currentVao != 0) {
		currentVao = 0;
		glBindVertexArray(0);
		GL_CHECK_PUSH_ERROR;
	}
}

}

