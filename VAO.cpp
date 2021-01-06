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

#include "VAO.h"

VAO::VAO(GLenum mode) : mode(mode) {
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

void VAO::SetAttribPointer(VBO& vbo, int location, unsigned count, GLenum type,
		bool normalized, unsigned offset, unsigned divisor) {
	glBindVertexArray(vaoID);
	glBindBuffer(vbo.target, vbo.vboID);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, count, type, normalized, vbo.vertexSize,
			(void*)(size_t)offset);
	glVertexAttribDivisor(location, divisor);
	glBindVertexArray(0);
	glBindBuffer(vbo.target, 0);
	if(divisor>0) {
		instances = std::max(instances, divisor*vbo.vertices);
	} else if(vbo.target == GL_ELEMENT_ARRAY_BUFFER) {
		drawArrays = false;
		sizeI = std::max(vbo.vertices, sizeI);
		typeElements = type;
	} else {
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
	glBindVertexArray(vaoID);
	void* offset = NULL;
	switch(typeElements) {
		case GL_UNSIGNED_BYTE:
			offset = (void*)(size_t)(start*1);
			break;
		case GL_UNSIGNED_SHORT:
			offset = (void*)(size_t)(start*2);
			break;
		case GL_UNSIGNED_INT:
			offset = (void*)(size_t)(start*4);
			break;
	}
	if(instances)
		glDrawElementsInstanced(mode, count, typeElements, offset, instances);
	else
		glDrawElements(mode, count, typeElements, offset);
	glBindVertexArray(0);
}

void VAO::SetSize(unsigned count) {
	if(drawArrays)
		sizeA = count;
	else
		sizeI = count;
}

