
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

void VAO::SetAttribPointer(VBO& vbo, int location, unsigned count, GLenum type, bool normalized, unsigned offset, unsigned divisor) {
	glBindVertexArray(vaoID);
	glBindBuffer(vbo.target, vbo.vboID);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, count, type, normalized, vbo.vertexSize, (void*)offset);
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
#warning glDrawElements::indices should be (void*)start or (void*)start*sizeof(typeElements)
	if(instances)
		glDrawElementsInstanced(mode, count, typeElements, (void*)start, instances);
	else
		glDrawElements(mode, count, typeElements, (void*)start);
	glBindVertexArray(0);
}

void VAO::SetSize(unsigned count) {
	if(drawArrays)
		sizeA = count;
	else
		sizeI = count;
}

