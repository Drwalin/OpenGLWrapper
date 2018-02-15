
#ifndef VAO_CPP
#define VAO_CPP

#include "VAO.h"

void VAO::SetAttribPointer( const int location, const unsigned int count, const GLenum type, const bool normalized, const unsigned int offset )
{
	if( generated )
	{
		glBindVertexArray( vaoID );
		//glBindBuffer( GL_ARRAY_BUFFER, vboID );
		glEnableVertexAttribArray( location );
		glVertexAttribPointer( location, count, type, normalized, vertexSize, (void*)offset );
	}
}

void VAO::AddVertex( const char * bytes )
{
	vertices.insert( vertices.end(), bytes, bytes + vertexSize );
}

void VAO::SetVertexSize( const unsigned int size )
{
	vertexSize = size;
}

bool VAO::IsGenerated()
{
	return generated;
}

void VAO::Generate()
{
	if( generated )
		Destroy();
	
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );
	
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, vertices.size(), vertices.begin(), GL_STATIC_DRAW );
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	generated = true;
}

void VAO::ClearVertices()
{
	vertices.clear();
}

void VAO::Draw( unsigned int beg, unsigned int end, const GLenum type ) const
{
	if( generated )
	{
		glBindVertexArray( vaoID );
		//glBindBuffer( GL_ARRAY_BUFFER, vboID );
		glDrawArrays( type, beg, end );
	}
}

void VAO::Draw( const GLenum type ) const
{
	Draw( type, 0, vertices.size() );
}

void VAO::Destroy()
{
	if( generated )
	{
		glDeleteVertexArrays( 1, &vaoID );
		glDeleteBuffers( 1, &vboID );
		generated = false;
		vaoID = 0;
		vboID = 0;
	}
}

VAO::VAO()
{
	vertexSize = 12;
	generated = false;
	vaoID = 0;
	vboID = 0;
}

VAO::~VAO()
{
	Destroy();
	vertices.clear();
}

#endif

