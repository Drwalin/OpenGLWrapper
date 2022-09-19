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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include "thirdparty/soil2/SOIL2.h"

#include "OpenGL.h"

namespace gl {
	enum TextureDataFormat : GLenum {
		R = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		BGR = GL_BGR,
		RGBA = GL_RGBA,
		BGRA = GL_BGRA,
		R_Int_ = GL_RED_INTEGER,
		RG_Int = GL_RG_INTEGER,
		RGB_Int = GL_RGB_INTEGER,
		BGR_Int = GL_BGR_INTEGER,
		RGBA_Int = GL_RGBA_INTEGER,
		BGRA_Int = GL_BGRA_INTEGER,
		StencilIndex = GL_STENCIL_INDEX,
		DepthComponent = GL_DEPTH_COMPONENT,
		DepthStencil = GL_DEPTH_STENCIL
	};

	enum TextureTarget : GLenum {
		Texture2D = GL_TEXTURE_2D,
		ProxyTexture2D = GL_PROXY_TEXTURE_2D,
		Texture1DArray = GL_TEXTURE_1D_ARRAY,
		ProxyTexture1DArray = GL_PROXY_TEXTURE_1D_ARRAY,
		TextureRectangle = GL_TEXTURE_RECTANGLE,
		ProxyTextureRectangle = GL_PROXY_TEXTURE_RECTANGLE,
		TextureCubeMapPositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		TextureCubeMapNegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		TextureCubeMapPositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		TextureCubeMapNegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		TextureCubeMapPositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		TextureCubeMapNegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		ProxyTextureCubeMap = GL_PROXY_TEXTURE_CUBE_MAP
	};
}

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
			gl::TextureTarget target, gl::TextureDataFormat internalformat,
			gl::TextureDataFormat dataformat, gl::DataType datatype);
	
	void Bind() const;
	unsigned int GetTexture() const;
	
	void Destroy();
	
	Texture();
	~Texture();
};

#endif

