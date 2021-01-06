/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Texture.h"

Texture::Texture() {
	textureID = 0;
	width = 0;
	height = 0;
}

Texture::~Texture() {
	Destroy();
}

bool Texture::Loaded() const {
	return (bool)textureID;
}

unsigned int Texture::GetWidth() const {
	return width;
}

unsigned int Texture::GetHeight() const {
	return height;
}

int Texture::Load(const char* fileName, int paramWrap, int paramFilter,
		bool generateMipMap) {
	unsigned char * image = SOIL_load_image(fileName, (int*)&width,
			(int*)&height, 0, SOIL_LOAD_RGBA);
	if(image==NULL && textureID) {
		glDeleteTextures(1, &textureID);
		textureID = width = height = 0;
		return 1;
	}
	
	UpdateTextureData(image, width, height, paramWrap, paramFilter,
			generateMipMap, GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	SOIL_free_image_data(image);
	return 0;
}

void Texture::UpdateTextureData(const void* data, unsigned w, unsigned h,
		int paramWrap, int paramFilter, bool generateMipMap, GLenum target,
		GLenum internalformat, GLenum dataformat, GLenum datatype) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(!textureID)
		glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	width = w;
	height = h;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, paramWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, paramWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, paramFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, paramFilter);
	
	glTexImage2D(target, 0, internalformat, width, height, 0, dataformat,
			datatype, data);
	
	if(generateMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() const {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

unsigned int Texture::GetTexture() const {
	return textureID;
}

void Texture::Destroy() {
	if(textureID) {
		glEnable(GL_TEXTURE_2D);
		glDeleteTextures(1, &textureID);
		width = 0;
		height = 0;
		textureID = 0;
	}
}

