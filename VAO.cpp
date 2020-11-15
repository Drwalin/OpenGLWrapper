
#include "VAO.h"


#include <cstdio>

VAO::VertexRef::VertexRef() :
	vao(NULL), id(0) {
}

VAO::VertexRef::VertexRef(VAO* vao, unsigned id) :
	vao(vao), id(id) {
}

VAO::VertexRef::VertexRef(VertexRef&& r) :
	vao(r.vao), id(r.id) {
}

VAO::VertexRef::VertexRef(const VertexRef& r) :
	vao(r.vao), id(r.id) {
}


void VAO::ReserveVertices(unsigned num) {
	unsigned s = (num+1)*vertexSize;
	if(vertices.size() < s)
		vertices.resize(s);
}

void VAO::SetAttribPointer(const int location, const unsigned int count, const GLenum type, const bool normalized, const unsigned int offset) {
	if(generated) {
		glBindVertexArray(vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, count, type, normalized, vertexSize, (void*)offset);
	}
}

void VAO::AddVertex(const void * bytes) {
	vertices.insert(vertices.end(), (unsigned char*)bytes, (unsigned char*)(bytes + vertexSize));
}

void VAO::SetVertexSize(const unsigned int size) {
	vertexSize = size;
}

bool VAO::IsGenerated() {
	return generated;
}

void VAO::Generate(const GLenum type) {
	if(generated) {
		glDeleteVertexArrays(1, &vaoID);
		glDeleteBuffers(1, &vboID);
		generated = false;
		vaoID = 0;
		vboID = 0;
	}
	
	verticesCount = vertices.size()/vertexSize;
	
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(unsigned char), &vertices.front(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	this->type = type;
	
	generated = true;
}

void VAO::ClearVertices() {
	vertices.clear();
	vertices.shrink_to_fit();
}

void VAO::Draw(unsigned int beg, unsigned int end) const {
	if(generated && beg<verticesCount) {
		glBindVertexArray(vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glDrawArrays(type, beg, std::min<unsigned>(end, verticesCount-beg));
	}
}

void VAO::Draw() const {
	Draw(0, verticesCount);
}

void VAO::Destroy() {
	if(generated) {
		glDeleteVertexArrays(1, &vaoID);
		glDeleteBuffers(1, &vboID);
		generated = false;
		vaoID = 0;
		vboID = 0;
		vertices.clear();
		verticesCount = 0;
	}
}

VAO::VAO() {
	vertexSize = 4;
	generated = false;
	vaoID = 0;
	vboID = 0;
	verticesCount = 0;
}

VAO::~VAO() {
	Destroy();
	vertices.clear();
	verticesCount = 0;
}
