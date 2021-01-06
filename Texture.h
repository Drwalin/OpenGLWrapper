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

