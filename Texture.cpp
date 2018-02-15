
#ifndef TEXTURE_CPP
#define TEXTURE_CPP

#include "Texture.h"

bool Texture::Loaded() const
{
	return (bool)textureID;
}

unsigned int Texture::GetWidth() const
{
	return width;
}

unsigned int Texture::GetHeight() const
{
	return height;
}

int Texture::Load( const char * file, const int paramWrap, const int paramFilter, const bool generateMipMap )
{
	glEnable( GL_TEXTURE_2D );
	
	if( textureID )
		Destroy();
	
	File file( file, "r" );
	if( !file.good() )
		return 1;
	file.close();
	
	glGenTextures( 1, &textureID );
	glBindTexture( GL_TEXTURE_2D, textureID );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, paramWrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, paramWrap );
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, paramFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, paramFilter );
	
	
	unsigned char * image = SOIL_load_image( file, &width, &height, 0, SOIL_LOAD_RGBA );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
	
	if( generateMipMap )
		glGenerateMipmap( GL_TEXTURE_2D );
	
	SOIL_free_image_data( image );
	
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Texture::Bind() const
{
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, textureID );
}

unsigned int Texture::GetTexture() const
{
	return textureID;
}

void Texture::Destroy()
{
	if( loaded )
	{
		glEnable( GL_TEXTURE_2D );
		glDeleteTextures( 1, &textureID );
		width = 0;
		height = 0;
		textureID = 0;
	}
}

Texture::Texture()
{
	textureID = 0;
	width = 0;
	height = 0;
}

Texture::~Texture()
{
	Destroy();
}

#endif

