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

#include "VBO.h"

VBO::VBO(unsigned vertexSize, GLenum target, GLenum usage) :
	vertexSize(vertexSize), target(target), usage(usage) {
	vboID = 0;
	glGenBuffers(1, &vboID);
}

VBO::~VBO() {
	if(vboID)
		glDeleteBuffers(1, &vboID);
}

void VBO::Generate() {
	vertices = buffer.size()/vertexSize;
	glBindVertexArray(0);
	glBindBuffer(target, vboID);
	glBufferData(target, buffer.size(), &buffer.front(), usage);
	glBindBuffer(target, 0);
}

void VBO::Update(unsigned beg, unsigned end) {
	end = std::min<unsigned>(end, vertices);
	if(beg >= end)
		return;
	unsigned offset = beg * vertexSize;
	unsigned size = (end - beg) * vertexSize;
	if(buffer.size() <= offset+size)
		return;
	glBindVertexArray(0);
	glBindBuffer(target, vboID);
	glBufferSubData(target, offset, size, &(buffer[offset]));
	glBindBuffer(target, 0);
}

void VBO::SetType(unsigned vertexSize, GLenum target, GLenum usage) {
	this->vertexSize = vertexSize;
	this->target = target;
	this->usage = usage;
}

void VBO::ClearHostBuffer() {
	buffer.clear();
	buffer.shrink_to_fit();
}

void VBO::FetchAllDataToHostFromGPU() {
	if(vboID) {
		glBindVertexArray(0);
		glBindBuffer(target, vboID);
		buffer.resize(vertexSize*vertices);
		glGetBufferSubData(target, 0, vertexSize*vertices, &buffer.front());
		glBindBuffer(target, 0);
	}
}

