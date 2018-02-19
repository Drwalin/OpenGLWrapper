
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include "C:\programowanie\cpp\OpenGL\SpartanJ-soil2-54073b423037\src\SOIL2\SOIL2.h"

#include "../lib/SourceHeader/File.h"

class Texture
{
private:
	
	unsigned int width, height;
	unsigned int textureID;
	
public:
	
	bool Loaded() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	
	int Load( const char * fileName, const int paramWrap, const int paramFilter, const bool generateMipMap );		// return 0 if no errors
	
	void Bind() const;
	unsigned int GetTexture() const;
	
	void Destroy();
	
	Texture();
	~Texture();
};

#endif

