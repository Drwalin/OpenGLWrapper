
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
	
public:
	
	void SetAttribPointer( const int location, const unsigned int count, const GLenum type, const bool normalized, const unsigned int offset );		// offset in bytes
	void AddVertex( const char * bytes );
	void SetVertexSize( const unsigned int size );
	
	bool IsGenerated();
	void Generate();
	void ClearVertices();
	
	void Draw( unsigned int beg, unsigned int end, const GLenum type ) const;
	void Draw( const GLenum type ) const;
	
	void Destroy();
	
	VAO();
	~VAO();
};

#endif

