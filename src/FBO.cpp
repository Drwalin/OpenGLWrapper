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

#include <cstdio>

#include "../include/openglwrapper/FBO.hpp"

namespace gl {

	FBO::FBO() {
		glGenFramebuffers(1, &fbo);
	}
	
	FBO::~FBO() {
		if(currentlyBoundFBO == this) {
			Unbind();
		}
		glDeleteFramebuffers(1, &fbo);
	}
	
	
	void FBO::AttachTexture(Texture* texture, FboAttachmentType attachmentType) {
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, texture->GetTexture(), 0);
	}
	
	void FBO::DetachTexture(FboAttachmentType attachmentType) {
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, 0, 0);
	}
	
	void FBO::AttachColor(Texture* texture, int colorId) {
		AttachTexture(texture, (FboAttachmentType)(ATTACHMENT_COLOR0+colorId));
	}
	
	void FBO::DetachColor(int colorId) {
		DetachTexture((FboAttachmentType)(ATTACHMENT_COLOR0+colorId));
	}
	
	void FBO::AttachDepth(Texture* texture) {
		AttachTexture(texture, ATTACHMENT_DEPTH);
	}
	
	void FBO::DetachDepth() {
		DetachTexture(ATTACHMENT_DEPTH);
	}
	
	
	void FBO::Viewport(int x, int y, int width, int height) {
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		glViewport(x, y, width, height);
	}
	
	void FBO::Clear(bool color, bool depth) {
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear((color?GL_COLOR_BUFFER_BIT:0) | (depth?GL_DEPTH_BUFFER_BIT:0));
	}
	
	void FBO::SetClearColor(glm::vec4 clearColor) {
		this->clearColor = clearColor;
	}

	
	FBO* FBO::currentlyBoundFBO = NULL;
	
	void FBO::Bind() {
		if(currentlyBoundFBO != this) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			currentlyBoundFBO = this;
		}
	}
	
	void FBO::Unbind() {
		if(currentlyBoundFBO) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			currentlyBoundFBO = NULL;
		}
	}
	
	
	GLenum FBO::CheckStatus() {
		Bind();
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
			return status;	
		return status;
	}

} // namespace gl

