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

#include <GL/glew.h>

#include "../include/openglwrapper/OpenGL.hpp"

#include "../include/openglwrapper/Sync.hpp"

namespace gl {
	Sync::Sync(void* glsync) : sync(glsync) {
	}
	
	Sync::~Sync() {
		Destroy();
	}
	
	Sync Sync::Fence() {
		return Sync(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
	}
	
	void Sync::Destroy() {
		if(sync) {
			glDeleteSync(static_cast<GLsync>(sync));
			sync = nullptr;
		}
	}
	
	bool Sync::IsDone() {
		if(sync) {
			GLint result = 0;
			glGetSynciv(static_cast<GLsync>(sync), GL_SYNC_STATUS,
					sizeof(uint32_t), nullptr, &result);
			return result == GL_SIGNALED;
		}
		return true;	
	}
	
	SyncWaitResult Sync::WaitClient(uint64_t timeoutNanoseconds) {
		if(sync) {
			GLenum ret = glClientWaitSync(static_cast<GLsync>(sync),
					GL_SYNC_FLUSH_COMMANDS_BIT, timeoutNanoseconds);
			GL_CHECK_PUSH_ERROR;
			switch(ret) {
			case GL_ALREADY_SIGNALED:
				return SYNC_DONE;
			case GL_TIMEOUT_EXPIRED:
				return SYNC_TIMEOUT;
			case GL_CONDITION_SATISFIED:
				return SYNC_DONE;
			}
		}
		return SYNC_NOT_EXISTS;
	}
	
	void Sync::WaitServer() {
		if(sync) {
			glWaitSync(static_cast<GLsync>(sync), 0, GL_TIMEOUT_IGNORED);
			glFlush();
		}
	}
} // namespace gl

