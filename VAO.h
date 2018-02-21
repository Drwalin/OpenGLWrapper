
#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

#include "lib/Array.cpp"

class VAO
{
private:
	
	int vertexSize;
	bool generated;
	unsigned int vaoID, vboID;
	Array < unsigned char > vertices;
	GLenum type;
	
public:
	
	Array < unsigned char >& AccessVertices();
	void SetAttribPointer( const int location, const unsigned int count, const GLenum type, const bool normalized, const unsigned int offset );		// offset in bytes
	void AddVertex( const void * bytes );
	void SetVertexSize( const unsigned int size );
	
	bool IsGenerated();
	void Generate( const GLenum type );
	void ClearVertices();
	
	void Draw( unsigned int beg, unsigned int end ) const;
	void Draw() const;
	
	void Destroy();
	
	VAO();
	~VAO();
};

#endif

