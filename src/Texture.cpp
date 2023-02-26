/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021-2023 Marek Zalewski aka Drwalin
 *
 *  OpenGLWrapper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenGLWrapper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../include/openglwrapper/Texture.hpp"

#include "../thirdparty/SOIL2/src/SOIL2/SOIL2.h"

namespace gl {

Texture::Texture() {
	textureID = 0;
	width = 0;
	height = 0;
	depth = 0;
}

Texture::~Texture() {
	Destroy();
}

bool Texture::Load(const char* fileName,
		bool generateMipMap, int forceChannelsCount) {
	return Load(fileName, generateMipMap,
			(gl::TextureSizedInternalFormat)gl::RGBA, forceChannelsCount);
}

bool Texture::Load(const char* fileName, bool generateMipMap,
		gl::TextureSizedInternalFormat forceSizedInternalFormat,
		int forceChannelsCount) {
	int channels = 0;
	uint8_t * image = LoadImageData(fileName, &width, &height, &channels,
			forceChannelsCount);
	if(image==NULL && textureID) {
		glDeleteTextures(1, &textureID);
		textureID = width = height = depth = 0;
		return false;
	}
	
	TextureDataFormat format = RGBA;
	switch(forceChannelsCount ? forceChannelsCount : channels) {
		case 1: format = RED; break;
		case 2: format = RG; break;
		case 3: format = RGB; break;
		case 4: format = RGBA; break;
		default:
			glDeleteTextures(1, &textureID);
			textureID = width = height = 0;
			return false;
	}
	
	UpdateTextureData(image, width, height, generateMipMap, gl::TEXTURE_2D,
			forceSizedInternalFormat, gl::RGBA, gl::UNSIGNED_BYTE);
	FreeImageData(image);
	depth = 1;
	return true;
}

void Texture::UpdateTextureData(const void* data, uint32_t w, uint32_t h,
		bool generateMipMap,
		gl::TextureTarget target,
		gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat,
		gl::DataType datatype) {
	
	this->target = target;
	if(!textureID)
		glCreateTextures(target, 1, &textureID);
	glBindTexture(target, textureID);
	
	width = w;
	height = h;
	depth = 1;
	
	glTexImage2D(target, 0, internalformat, width, height, 0, dataformat,
			datatype, data);
	
	if(generateMipMap)
		glGenerateMipmap(target);
	else {
		MinFilter(gl::NEAREST);
	}
}

void Texture::InitTextureEmpty(uint32_t w, uint32_t h, 
		gl::TextureTarget target, gl::TextureSizedInternalFormat internalformat) {
	UpdateTextureData(NULL, w, h, false, target, internalformat, gl::RGBA, gl::UNSIGNED_BYTE);
	/*
	this->target = target;
	if(!textureID)
		glCreateTextures(target, 1, &textureID);
	glBindTexture(target, textureID);
	
	width = w;
	height = h;
	depth = 1;
	
	glTexStorage2D(target, 1, internalformat, w, h);
	
	MinFilter(gl::NEAREST);
	*/
}


		
void Texture::UpdateTextureData3(const void* data, uint32_t w, uint32_t h, uint32_t d,
		bool generateMipMap,
		gl::TextureTarget target, gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
}

void Texture::InitTextureEmpty3(uint32_t w, uint32_t h, uint32_t d ,
		gl::TextureTarget target, gl::TextureSizedInternalFormat internalformat) {
	
}



void Texture::UpdateTextureData1(const void* data, uint32_t w,
		bool generateMipMap,
		gl::TextureTarget target, gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	
	
}

void Texture::InitTextureEmpty1(uint32_t w, 
		gl::TextureTarget target, gl::TextureSizedInternalFormat internalformat) {
	
}




void Texture::MinFilter(TextureMinFilter filter) {
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture::MagFilter(TextureMagFilter filter) {
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::WrapX(TextureWrapParam param) {
	glTexParameteri(target, GL_TEXTURE_WRAP_S, param);
}

void Texture::WrapY(TextureWrapParam param) {
	glTexParameteri(target, GL_TEXTURE_WRAP_T, param);
}

void Texture::WrapZ(TextureWrapParam param) {
	glTexParameteri(target, GL_TEXTURE_WRAP_R, param);
}

void Texture::SetDefaultParamPixelartClampBorderNoMipmap() {
	WrapX(gl::CLAMP_TO_BORDER);
	WrapY(gl::CLAMP_TO_BORDER);
	MinFilter(gl::NEAREST);
	MagFilter(gl::MAG_NEAREST);
}

void Texture::Bind() const {
	glBindTexture(target, textureID);
}

uint32_t Texture::GetTexture() const {
	return textureID;
}

void Texture::Unbind() {
	glBindTexture(target, 0);
}

void Texture::Destroy() {
	if(textureID) {
		glDeleteTextures(1, &textureID);
		width = 0;
		height = 0;
		textureID = 0;
	}
}

uint8_t* Texture::LoadImageData(const char* fileName, int* width, int* channels,
		int* height, int forceChannelsCount) {
	return SOIL_load_image(fileName, width, height, channels,
			forceChannelsCount);
}

void Texture::FreeImageData(uint8_t* imageData) {
	SOIL_free_image_data(imageData);
}

}

