/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdio>

#include "OpenGL.h"
#include "VAO.h"
#include "VBO.h"

namespace gl {

VAO::VAO(gl::VertexMode mode) : mode(mode) {
	sizeI = 0;
	sizeA = 0;
	vaoID = 0;
	glBindVertexArray(0);
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(0);
	drawArrays = true;
	instances = 0;
}

VAO::~VAO() {
	glDeleteVertexArrays(1, &vaoID);
}

void VAO::SetAttribPointer(VBO& vbo, int location, unsigned count,
		gl::DataType type, bool normalized, unsigned offset, unsigned divisor) {
	glBindVertexArray(vaoID);
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(vbo.target, vbo.vboID);
	GL_CHECK_PUSH_ERROR;
	if(vbo.target != gl::ELEMENT_ARRAY_BUFFER && vbo.target != gl::DRAW_INDIRECT_BUFFER) {
		glEnableVertexAttribArray(location);
		fprintf(stderr, " vertex attrib array location = %i\n", location);
	GL_CHECK_PUSH_ERROR;
		glVertexAttribPointer(location, count, type, normalized, vbo.vertexSize,
				(void*)(size_t)offset);
	GL_CHECK_PUSH_ERROR;
		glVertexAttribDivisor(location, divisor);
	GL_CHECK_PUSH_ERROR;
	}
	GL_CHECK_PUSH_ERROR;
	glBindVertexArray(0);
	GL_CHECK_PUSH_ERROR;
	glBindBuffer(vbo.target, 0);
	GL_CHECK_PUSH_ERROR;
	if(divisor>0) {
		instances = std::max(instances, divisor*vbo.vertices);
	} else if(vbo.target == gl::ELEMENT_ARRAY_BUFFER) {
		drawArrays = false;
		sizeI = std::max(vbo.vertices, sizeI);
		typeElements = type;
	} else if(vbo.target != gl::DRAW_INDIRECT_BUFFER) {
		sizeA = std::max(vbo.vertices, sizeA);
	}
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
	glBindVertexArray(vaoID);
	if(instances)
		glDrawArraysInstanced(mode, start, count, instances);
	else
		glDrawArrays(mode, start, count);
	glBindVertexArray(0);
}

void VAO::DrawElements(unsigned start, unsigned count) {
	GL_CHECK_PUSH_ERROR;
	glBindVertexArray(vaoID);
	GL_CHECK_PUSH_ERROR;
	void* offset = NULL;
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
	GL_CHECK_PUSH_ERROR;
		glDrawElementsInstanced(mode, count, typeElements, offset, instances);
	GL_CHECK_PUSH_ERROR;
	} else {
	GL_CHECK_PUSH_ERROR;
		glDrawElements(mode, count, typeElements, offset);
	GL_CHECK_PUSH_ERROR;
	}
	GL_CHECK_PUSH_ERROR;
	glBindVertexArray(0);
	GL_CHECK_PUSH_ERROR;
}

void VAO::DrawMultiElementsIndirect(void* indirect, int drawCount, size_t stride) {
	glBindVertexArray(vaoID);
	switch(typeElements) {
		case gl::UNSIGNED_BYTE:
		case gl::UNSIGNED_SHORT:
		case gl::UNSIGNED_INT:
			break;
		default:
			// TODO: error
			printf(" error in VAO::DrawElements: unusable element internal indexing type\n");
	}
	glMultiDrawElementsIndirect(mode, typeElements, indirect, drawCount, stride);
	glBindVertexArray(0);
}

void VAO::SetSize(unsigned count) {
	if(drawArrays)
		sizeA = count;
	else
		sizeI = count;
}

}

