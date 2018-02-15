#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include "lib/File.h"
#include "lib/String.h"
#include "lib/Map.cpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
	
    unsigned int program;
	
public:
	
    int Load( const char * vertexPath, const char * geometryPath, const char * fragmentPath );
    void Use();
    unsigned int GetProgram();
	
	int GetLocation( const char * name ) const;
	
	void setBool( const int location, bool value ) const;
	void setInt( const int location, int value ) const;
	void setFloat( const int location, float value ) const;
	void setVec2( const int location, const glm::vec2 &value ) const;
	void setVec3( const int location, const glm::vec3 &value ) const;
	void setVec4( const int location, const glm::vec4 &value ) const;
	void setMat2( const int location, const glm::mat2 &mat ) const;
	void setMat3( const int location, const glm::mat3 &mat ) const;
	void setMat4( const int location, const glm::mat4 &mat ) const;
	
	Shader();
	~Shader();
};

#endif
