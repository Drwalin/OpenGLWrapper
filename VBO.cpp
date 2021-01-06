
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

