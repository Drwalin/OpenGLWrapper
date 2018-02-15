
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include "C:\programowanie\cpp\OpenGL\SpartanJ-soil2-54073b423037\src\SOIL2\SOIL2.h"

#include "lib/File.h"

class Texture
{
private:
	
	unsigned int width, height;
	unsigned int textureID;
	
public:
	
	bool Loaded() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	
	int Load( const char * file, const int paramWrap, const int paramFilter, const bool generateMipMap );
	
	void Bind() const;
	unsigned int GetTexture() const;
	
	void Destroy();
	
	Texture();
	~Texture();
};

#endif

