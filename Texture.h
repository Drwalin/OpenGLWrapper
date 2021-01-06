
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SOIL2\SOIL2.h>

class Texture {
private:
	
	unsigned int width, height;
	unsigned int textureID;
	
public:
	
	bool Loaded() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	
	int Load(const char* fileName, int paramWrap, int paramFilter,
			bool generateMipMap);		// return 0 if no errors
	void UpdateTextureData(const void* data, unsigned w, unsigned h,
			int paramWrap, int paramFilter, bool generateMipMap,
			GLenum target, GLenum internalformat, GLenum dataformat,
			GLenum datatype);
	
	void Bind() const;
	unsigned int GetTexture() const;
	
	void Destroy();
	
	Texture();
	~Texture();
};

#endif

