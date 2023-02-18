/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2023 Marek Zalewski aka Drwalin
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

#ifndef FBO_H
#define FBO_H

#include <GL/glew.h>

#include <vector>

#include "Texture.hpp"
#include "OpenGL.hpp"

namespace gl {
	enum FboAttachmentType : GLenum {
	};

	class FBO {
	public:
		
		FBO();
		~FBO();
		
		void AttachTexture(Texture* texture, FboAttachmentType  attachmentType);
		void DetachTexture(Texture* texture, FboAttachmentType  attachmentType);
		
		void Viewport(int x, int y, int width, int height);
		void Clear(bool color, bool depth);
		void SetClearColor(glm::vec3 clearColor);
		
		void Bind();
		void Unbind();
		
	private:
	public:
		
		int fbo;
		int width, height;
		glm::vec4 clearColor;
		
		static FBO* currentlyBoundFBO;
	};
}

#endif

