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

#include <cstdio>
#include <string>
#include <mutex>
#include <set>
#include <map>

#include "../thirdparty/SOIL2/src/SOIL2/SOIL2.h"

#include "../include/openglwrapper/Texture.hpp"

namespace gl {
	
static uint32_t GetBytesPerFormat(TextureSizedInternalFormat format) {
	static std::map<TextureSizedInternalFormat, int> m{
		{R8, 1},
		{R8_SNORM, 1},
		{R16, 2},
		{R16_SNORM, 2},
		{RG8, 2},
		{RG8_SNORM, 2},
		{RG16, 4},
		{RG16_SNORM, 4},
		{R3_G3_B2, 1},
		{RGB4, 2},
		{RGB5, 2},
		{RGB8, 3},
		{RGB8_SNORM, 3},
		{RGB10, 4},
		{RGB12, 5},
		{RGB16_SNORM,6 },
		{RGBA2, 1},
		{RGBA4, 2},
		{RGB5_A1, 2},
		{RGBA8, 4},
		{RGBA8_SNORM, 4},
		{RGB10_A2, 4},
		{RGB10_A2UI, 4},
		{RGBA12, 6},
		{RGBA16, 8},
		{SRGB8, 4},
		{SRGB8_ALPHA8, 4},
		{R16F, 2},
		{RG16F, 4},
		{RGB16F, 6},
		{RGBA16F, 8},
		{R32F, 4},
		{RG32F, 8},
		{RGB32F, 12},
		{RGBA32F, 16},
		{R11F_G11F_B10F, 4},
		{RGB9_E5, 4},
		{R8I, 1},
		{R8UI, 1},
		{R16I, 2},
		{R16UI, 2},
		{R32I, 4},
		{R32UI, 4},
		{RG8I, 2},
		{RG8UI, 2},
		{RG16I, 4},
		{RG16UI, 4},
		{RG32I, 8},
		{RG32UI, 8},
		{RGB8I, 3},
		{RGB8UI, 3},
		{RGB16I, 6},
		{RGB16UI, 6},
		{RGB32I, 12},
		{RGB32UI, 12},
		{RGBA8I, 4},
		{RGBA8UI, 4},
		{RGBA16I, 8},
		{RGBA16UI, 8},
		{RGBA32I, 16},
		{RGBA32UI, 16},
		{(TextureSizedInternalFormat)RGBA, 4},
		{(TextureSizedInternalFormat)GL_DEPTH_COMPONENT32F, 4},
		{(TextureSizedInternalFormat)GL_DEPTH_COMPONENT32, 4},
		{(TextureSizedInternalFormat)GL_DEPTH_COMPONENT16, 2},
		
		{DEPTH24_STENCIL8, 4},
	};
	
	if(m.find(format) == m.end()) {
		throw "Trying to find size in bytes of unknown gl::TextureSizedInternalFormat.";
	}
	return m[format];
}
	
static std::set<Texture*> allTextures;
static std::mutex mutex;

void Texture::UpdateVramUsage() {
	vramUsage = width*height*depth * GetBytesPerFormat(internalFormat);
	if(hasMipmaps)
		vramUsage = (11 * vramUsage) / 8;
}

Texture::Texture() {
	textureID = 0;
	width = 0;
	height = 0;
	depth = 0;
	vramUsage = 0;
	std::lock_guard<std::mutex> lock(mutex);
	allTextures.insert(this);
	hasMipmaps = false;
}

Texture::~Texture() {
	Destroy();
	std::lock_guard<std::mutex> lock(mutex);
	allTextures.erase(this);
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
	int32_t w, h;
	uint8_t * image = LoadImageData(fileName, &w, &h, &channels,
			forceChannelsCount);
	if(image==nullptr && textureID) {
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
	
	Generate2(TEXTURE_2D, w, h, forceSizedInternalFormat);
	Update2(image, 0, 0, w, h, 0, format, gl::UNSIGNED_BYTE);
	if(generateMipMap)
		GenerateMipmaps();
	
	FreeImageData(image);
	depth = 1;
	return true;
}



void Texture::Generate1(gl::TextureTarget target,
		uint32_t w,
		gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	if(textureID && target != this->target) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
	GL_CHECK_PUSH_PRINT_ERROR;
	
	if(!textureID)
		glCreateTextures(target, 1, &textureID);
	GL_CHECK_PUSH_PRINT_ERROR;
	this->target = target;
	glBindTexture(target, textureID);
	GL_CHECK_PUSH_PRINT_ERROR;
	
	this->width = w;
	this->height = 1;
	this->depth = 1;
	this->internalFormat = internalformat;
	
	glTexImage1D(target, 0, internalformat, w, 0,
			dataformat, datatype, nullptr);
	GL_CHECK_PUSH_PRINT_ERROR;
	
	MinFilter(gl::NEAREST);
	GL_CHECK_PUSH_PRINT_ERROR;
	UpdateVramUsage();
}

void Texture::Update1(const void* pixels,
		uint32_t x,
		uint32_t w,
		uint32_t level,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	glTextureSubImage1D(textureID, level, x, w,
			dataformat, datatype, pixels);
	GL_CHECK_PUSH_PRINT_ERROR;
}

void Texture::Fetch1(void* pixels,
		uint32_t x,
		uint32_t w,
		uint32_t level,
		gl::TextureDataFormat dataformat, gl::DataType datatype,
		uint32_t pixelsBufferSize) {
	glGetTextureSubImage(textureID, level, x, 0, 0, w, 1, 1,
			dataformat, datatype, pixelsBufferSize, pixels);
	GL_CHECK_PUSH_PRINT_ERROR;
}


void Texture::Generate2(gl::TextureTarget target,
		uint32_t w, uint32_t h,
		gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	GL_CHECK_PUSH_PRINT_ERROR;
	if(textureID && target != this->target) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
	GL_CHECK_PUSH_PRINT_ERROR;
	
	this->target = target;
	if(!textureID)
		glCreateTextures(target, 1, &textureID);
	GL_CHECK_PUSH_PRINT_ERROR;
	glBindTexture(target, textureID);
	GL_CHECK_PUSH_PRINT_ERROR;
	
	this->width = w;
	this->height = h;
	this->depth = 1;
	this->internalFormat = internalformat;
	
	glTexImage2D(target, 0, internalformat, w, h, 0,
			dataformat, datatype, nullptr);
	GL_CHECK_PUSH_PRINT_ERROR;
	
	MinFilter(gl::NEAREST);
	MagFilter(gl::MAG_NEAREST);
	GL_CHECK_PUSH_PRINT_ERROR;
	UpdateVramUsage();
}

void Texture::Update2(const void* pixels,
		uint32_t x, uint32_t y,
		uint32_t w, uint32_t h,
		uint32_t level,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	glTextureSubImage2D(textureID, level, x, y, w, h,
			dataformat, datatype, pixels);
	GL_CHECK_PUSH_PRINT_ERROR;
}

void Texture::Fetch2(void* pixels,
		uint32_t x, uint32_t y,
		uint32_t w, uint32_t h,
		uint32_t level,
		gl::TextureDataFormat dataformat, gl::DataType datatype,
		uint32_t pixelsBufferSize) {
	glGetTextureSubImage(textureID, level, x, y, 0, w, h, 1,
			dataformat, datatype, pixelsBufferSize, pixels);
	GL_CHECK_PUSH_PRINT_ERROR;
}


void Texture::Generate3(gl::TextureTarget target,
		uint32_t w, uint32_t h, uint32_t d,
		gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	if(textureID && target != this->target) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
	GL_CHECK_PUSH_PRINT_ERROR;
	
	if(!textureID)
		glCreateTextures(target, 1, &textureID);
	GL_CHECK_PUSH_PRINT_ERROR;
	this->target = target;
	glBindTexture(target, textureID);
	GL_CHECK_PUSH_PRINT_ERROR;
	
	this->width = w;
	this->height = h;
	this->depth = d;
	this->internalFormat = internalformat;
	
	glTexImage3D(target, 0, internalformat, w, h, d, 0,
			dataformat, datatype, nullptr);
	GL_CHECK_PUSH_PRINT_ERROR;
	
	MinFilter(gl::NEAREST);
	MagFilter(gl::MAG_NEAREST);
	GL_CHECK_PUSH_PRINT_ERROR;
	UpdateVramUsage();
}

void Texture::Update3(const void* pixels,
		uint32_t x, uint32_t y, uint32_t z,
		uint32_t w, uint32_t h, uint32_t d,
		uint32_t level,
		gl::TextureDataFormat dataformat, gl::DataType datatype) {
	glTextureSubImage3D(textureID, level, x, y, z, w, h, d,
			dataformat, datatype, pixels);
	GL_CHECK_PUSH_PRINT_ERROR;
}

void Texture::Fetch3(void* pixels,
		uint32_t x, uint32_t y, uint32_t z,
		uint32_t w, uint32_t h, uint32_t d,
		uint32_t level,
		gl::TextureDataFormat dataformat, gl::DataType datatype,
		uint32_t pixelsBufferSize) {
	glGetTextureSubImage(textureID, level, x, y, z, w, h, d,
			dataformat, datatype, pixelsBufferSize, pixels);
	GL_CHECK_PUSH_PRINT_ERROR;
}



void Texture::UpdateTextureData(const void* pixels, uint32_t w, uint32_t h,
		bool generateMipMap,
		gl::TextureTarget target,
		gl::TextureSizedInternalFormat internalformat,
		gl::TextureDataFormat dataformat,
		gl::DataType datatype) {
	Generate2(target, w, h, internalformat, dataformat, datatype);
	if(pixels) {
		Update2(pixels, 0, 0, w, h, 0, dataformat, datatype);
	}
	
	if(generateMipMap) {
		GenerateMipmaps();
	} else {
		MinFilter(gl::NEAREST);
		MagFilter(gl::MAG_NEAREST);
	}
}

void Texture::InitTextureEmpty(uint32_t w, uint32_t h, 
		gl::TextureTarget target, gl::TextureSizedInternalFormat internalformat) {
	UpdateTextureData(nullptr, w, h, false, target, internalformat, gl::RGBA, gl::UNSIGNED_BYTE);
}



void Texture::GenerateMipmaps() {
	glGenerateTextureMipmap(textureID);
	hasMipmaps = true;
	UpdateVramUsage();
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
	WrapX(gl::MIRRORED_REPEAT);
	WrapY(gl::MIRRORED_REPEAT);
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

void Texture::BindImage(uint32_t unit, int32_t level, bool array,
		int arrayLayerId, bool read, bool write, GLenum format) {
	glBindImageTexture(unit, textureID, level, array, arrayLayerId,
			read&&write ? GL_READ_WRITE : read ? GL_READ_ONLY : GL_WRITE_ONLY,
			format);
	GL_CHECK_PUSH_ERROR;
}

void Texture::Destroy() {
	if(textureID) {
		glDeleteTextures(1, &textureID);
		width = 0;
		height = 0;
		textureID = 0;
	}
	vramUsage = 0;
	hasMipmaps = false;
}

uint8_t* Texture::LoadImageData(const char* fileName, int* width, int* height,
		int* channels, int forceChannelsCount) {
	return SOIL_load_image(fileName, width, height, channels,
			forceChannelsCount);
}

void Texture::FreeImageData(uint8_t* imageData) {
	SOIL_free_image_data(imageData);
}

uint64_t Texture::CountAllTextureMemoryUsage() {
	std::lock_guard<std::mutex> lock(mutex);
	uint64_t bytes = 0;
	for(auto v : allTextures) {
		if(v->GetTexture()) {
			bytes += v->vramUsage;
		}
	}
	return bytes;
}

}

