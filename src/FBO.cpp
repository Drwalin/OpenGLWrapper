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
		fbo = 0;
	}
	
	void FBO::Destroy() {
		if(currentlyBoundFBO == this) {
			Unbind();
		}
		glDeleteFramebuffers(1, &fbo);
		GL_CHECK_PUSH_ERROR;
		fbo = 0;
	}
	
	FBO::~FBO() {
		Destroy();
	}
	
	
	void FBO::AttachTexture(Texture* texture, FboAttachmentType attachmentType,
			uint32_t bindLocation) {
		SimpleBind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D,
				texture->GetTexture(), 0);
		GL_CHECK_PUSH_ERROR;
		if(attachmentType >= ATTACHMENT_COLOR0
				&& attachmentType <= ATTACHMENT_COLOR15) {
			if(attachmentBuffers.size() <= bindLocation) {
				attachmentBuffers.resize(bindLocation+1, ATTACHMENT_NONE);
			}
			attachmentBuffers[bindLocation] = attachmentType;
		}
	}
	
	void FBO::DetachTexture(FboAttachmentType attachmentType) {
		SimpleBind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, 0, 0);
		GL_CHECK_PUSH_ERROR;
	}
	
	void FBO::AttachColor(Texture* texture, int colorId, uint32_t bindLocation) {
		AttachTexture(texture, (FboAttachmentType)(ATTACHMENT_COLOR0+colorId), bindLocation);
	}
	
	void FBO::DetachColor(int colorId) {
		DetachTexture((FboAttachmentType)(ATTACHMENT_COLOR0+colorId));
	}
	
	void FBO::AttachDepth(Texture* texture) {
		AttachTexture(texture, ATTACHMENT_DEPTH, 0);
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
		GL_CHECK_PUSH_ERROR;
	}
	
	void FBO::Clear(bool color, bool depth) {
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		GL_CHECK_PUSH_ERROR;
		glClear((color?GL_COLOR_BUFFER_BIT:0) | (depth?GL_DEPTH_BUFFER_BIT:0));
		GL_CHECK_PUSH_ERROR;
	}
	
	void FBO::SetClearColor(glm::vec4 clearColor) {
		this->clearColor = clearColor;
	}

	
	
	FBO* FBO::currentlyBoundFBO = nullptr;
	
	void FBO::SimpleBind() {
		if(fbo == 0) {
			glCreateFramebuffers(1, &fbo);
			GL_CHECK_PUSH_ERROR;
		}
		if(currentlyBoundFBO != this) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			currentlyBoundFBO = this;
			GL_CHECK_PUSH_ERROR;
		}
	}
	
	void FBO::Bind() {
		SimpleBind();
		glDrawBuffers(attachmentBuffers.size(), (GLenum*)&(attachmentBuffers[0]));
		GL_CHECK_PUSH_ERROR;
	}
	
	void FBO::Unbind() {
		if(currentlyBoundFBO) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			GL_CHECK_PUSH_ERROR;
			currentlyBoundFBO = nullptr;
		}
	}
	
	
	GLenum FBO::CheckStatus() {
		SimpleBind();
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		GL_CHECK_PUSH_ERROR;
		if(status != GL_FRAMEBUFFER_COMPLETE)
			return status;	
		return status;
	}

} // namespace gl

