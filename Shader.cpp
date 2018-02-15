#ifndef SHADER_CPP
#define SHADER_CPP

#include "Shader.h"

int Shader::Load( const char * vertexPath, const char * geometryPath, const char * fragmentPath )
{
	// 1. Retrieve the vertex/fragment source code from filePath
	File gShaderFile;
	File vShaderFile;
	File fShaderFile;
	 
	char *gShaderCode = NULL;
	char *vShaderCode = NULL; 
	char *fShaderCode = NULL;
	
	{
		// Open files
		if( geometryPath )
			gShaderFile.open( geometryPath, "r" );
		vShaderFile.open( vertexPath, "r" );
		fShaderFile.open( fragmentPath, "r" );
		if( !vShaderFile.good() || !fShaderFile.good() || ( geometryPath && !gShaderFile.good() ) )
		{
			printf( "\n ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ  v%i - f%i - g%i ", (int)vShaderFile.good(), (int)fShaderFile.good(), (int)gShaderFile.good() );
			return 1;
		}
		else
		{
			// Read file's buffer contents into streams
			unsigned long long int glen = 0;
			if( geometryPath )
				glen = gShaderFile.getfilesize();
			unsigned long long int vlen = vShaderFile.getfilesize();
			unsigned long long int flen = fShaderFile.getfilesize();
			
			if( geometryPath )
			{
				gShaderCode = (char*)malloc( glen+1 );
				gShaderCode[glen] = 0;
			}
			vShaderCode = (char*)malloc( vlen+1 );
			vShaderCode[vlen] = 0;
			fShaderCode = (char*)malloc( flen+1 );
			fShaderCode[flen] = 0;
			
			if( geometryPath )
				gShaderFile.read( gShaderCode, glen );
			vShaderFile.read( vShaderCode, vlen );
			fShaderFile.read( fShaderCode, flen );
			
			// close file handlers
			if( geometryPath )
				gShaderFile.close( );
			vShaderFile.close( );
			fShaderFile.close( );
		}
	}
	
	// 2. Compile shaders
	GLuint geometry, vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	
	// Geometry Shader
	if( geometryPath )
	{
		geometry = glCreateShader( GL_GEOMETRY_SHADER );
		glShaderSource( geometry, 1, &gShaderCode, NULL );
		glCompileShader( geometry );
		// Print compile errors if any
		glGetShaderiv( geometry, GL_COMPILE_STATUS, &success );
		if ( !success )
		{
			glGetShaderInfoLog( geometry, 512, NULL, infoLog );
			printf( "\n ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n %c", infoLog );
			return 2;
		}
	}
	
	// Vertex Shader
	vertex = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertex, 1, &vShaderCode, NULL );
	glCompileShader( vertex );
	// Print compile errors if any
	glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( vertex, 512, NULL, infoLog );
		printf( "\n ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %c", infoLog );
		return 3;
	}
	
	// Fragment Shader
	fragment = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragment, 1, &fShaderCode, NULL );
	glCompileShader( fragment );
	// Print compile errors if any
	glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( fragment, 512, NULL, infoLog );
		printf( "\n ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %c", infoLog );
		return 4;
	}
	
	// Shader program
	this->program = glCreateProgram( );
	if( geometryPath )
		glAttachShader( this->program, geometry );
	glAttachShader( this->program, vertex );
	glAttachShader( this->program, fragment );
	glLinkProgram( this->program );
	// Print linking errors if any
	glGetProgramiv( this->program, GL_LINK_STATUS, &success );
	if (!success)
	{
		glGetProgramInfoLog( this->program, 512, NULL, infoLog );
		printf( "\n ERROR::SHADER::PROGRAM::LINKING_FAILED\n %c", infoLog );
		return 5;
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	if( geometryPath )
		glDeleteShader( geometry );
	glDeleteShader( vertex );
	glDeleteShader( fragment );
	
	if( geometryPath )
		free( gShaderCode );
	free( vShaderCode );
	free( fShaderCode );
	
	printf( "\n Shader compiled!" );
	return 0;
}
	
void Shader::Use()
{
	glUseProgram( this->program );
}

unsigned int Shader::GetProgram()
{
	return this->program;
}

int Shader::GetLocation( const char * name ) const
{
	return glGetUniformLocation( this->program, name );
}

void Shader::setBool( const int location, bool value ) const
{		 
	glUniform1i( location, (int)value ); 
}
void Shader::setInt( const int location, int value ) const
{ 
	glUniform1i( location, value ); 
}
void Shader::setFloat( const int location, float value ) const
{ 
	glUniform1f( location, value ); 
}
void Shader::setVec2( const int location, const glm::vec2 &value ) const
{ 
	glUniform2fv( location, 1, &value[0] );
}
void Shader::setVec3( const int location, const glm::vec3 &value ) const
{ 
	glUniform3fv( location, 1, &value[0] );
}
void Shader::setVec4( const int location, const glm::vec4 &value ) const
{ 
	glUniform4fv( location, 1, &value[0] );
}
void Shader::setMat2( const int location, const glm::mat2 &mat ) const
{
	glUniformMatrix2fv( location, 1, GL_FALSE, &mat[0][0] );
}
void Shader::setMat3( const int location, const glm::mat3 &mat ) const
{
	glUniformMatrix3fv( location, 1, GL_FALSE, &mat[0][0] );
}
void Shader::setMat4( const int location, const glm::mat4 &mat ) const
{
	glUniformMatrix4fv( location, 1, GL_FALSE, &mat[0][0] );
}

Shader::Shader()
{
	this->program = 0;
}

Shader::~Shader()
{
	this->program = 0;
}

#endif
