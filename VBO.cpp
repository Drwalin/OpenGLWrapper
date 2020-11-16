
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

