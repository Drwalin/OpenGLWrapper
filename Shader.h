#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include "../lib/SourceHeader/File.h"
#include "../lib/SourceHeader/String.h"
#include "../lib/SourceCode/Map.cpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
	
    unsigned int program;
	
public:
	
    int Load( const char * vertexPath, const char * geometryPath, const char * fragmentPath );		// return 0 if no errors
    void Use();
    unsigned int GetProgram();
	
	int GetUniformLocation( const char * name ) const;
	int GetAttributeLocation( const char * name ) const;
	
	void SetBool( const int location, bool value ) const;
	void SetInt( const int location, int value ) const;
	void SetInt( const int location, const Array < int >& array ) const;
	void SetInt( const int location, const Array < unsigned int >& array ) const;
	void SetFloat( const int location, float value ) const;
	void SetFloat( const int location, const Array < float >& array ) const;
	void SetVec2( const int location, const glm::vec2 &value ) const;
	void SetVec3( const int location, const glm::vec3 &value ) const;
	void SetVec3( const int location, const Array < glm::vec3 >& array ) const;
	void SetVec4( const int location, const glm::vec4 &value ) const;
	void SetMat2( const int location, const glm::mat2 &mat ) const;
	void SetMat3( const int location, const glm::mat3 &mat ) const;
	void SetMat4( const int location, const glm::mat4 &mat ) const;
	void SetMat4( const int location, const Array < glm::mat4 >& array ) const;
	
	Shader();
	~Shader();
};

#endif
