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

#ifndef OGLW_OPEN_GL_ENGINE_HPP
#define OGLW_OPEN_GL_ENGINE_HPP

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gl {
	void OpenGLKeyCallback(GLFWwindow* window, int key, int scancode, int action,
			int mode);
	void OpenGLScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	void OpenGLMouseCallback(GLFWwindow* window, double xPos, double yPos);
	void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);
	void OpenGLMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	enum DataType : GLenum {
		BYTE = GL_BYTE,
		UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
		SHORT = GL_SHORT,
		UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
		INT = GL_INT,
		UNSIGNED_INT = GL_UNSIGNED_INT,
		
		HALF_FLOAT = GL_HALF_FLOAT,
		FLOAT = GL_FLOAT,
		DOUBLE = GL_DOUBLE,
		
		UNSIGNED_BYTE_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
		UNSIGNED_BYTE_2_3_3_REV = GL_UNSIGNED_BYTE_2_3_3_REV,
		UNSIGNED_SHORT_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
		UNSIGNED_SHORT_5_6_5_REV = GL_UNSIGNED_SHORT_5_6_5_REV,
		UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
		UNSIGNED_SHORT_4_4_4_4_REV = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
		UNSIGNED_SHORT_1_5_5_5_REV = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		UNSIGNED_INT_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
		UNSIGNED_INT_8_8_8_8_REV = GL_UNSIGNED_INT_8_8_8_8_REV,
		UNSIGNED_INT_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
		UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
		
		UNSIGNED_INT_24_8 = GL_UNSIGNED_INT_24_8,
	};
	
	enum MemoryBarriers : GLbitfield {
		VERTEX_ATTRIB_ARRAY_BARRIER_BIT = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
		ELEMENT_ARRAY_BARRIER_BIT = GL_ELEMENT_ARRAY_BARRIER_BIT,
		UNIFORM_BARRIER_BIT = GL_UNIFORM_BARRIER_BIT,
		TEXTURE_FETCH_BARRIER_BIT = GL_TEXTURE_FETCH_BARRIER_BIT,
		SHADER_IMAGE_ACCESS_BARRIER_BIT = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
		COMMAND_BARRIER_BIT = GL_COMMAND_BARRIER_BIT,
		PIXEL_BUFFER_BARRIER_BIT = GL_PIXEL_BUFFER_BARRIER_BIT,
		TEXTURE_UPDATE_BARRIER_BIT = GL_TEXTURE_UPDATE_BARRIER_BIT,
		BUFFER_UPDATE_BARRIER_BIT = GL_BUFFER_UPDATE_BARRIER_BIT,
		CLIENT_MAPPED_BUFFER_BARRIER_BIT = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
		FRAMEBUFFER_BARRIER_BIT = GL_FRAMEBUFFER_BARRIER_BIT,
		TRANSFORM_FEEDBACK_BARRIER_BIT = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
		ATOMIC_COUNTER_BARRIER_BIT = GL_ATOMIC_COUNTER_BARRIER_BIT,
		SHADER_STORAGE_BARRIER_BIT = GL_SHADER_STORAGE_BARRIER_BIT,
		QUERY_BUFFER_BARRIER_BIT = GL_QUERY_BUFFER_BARRIER_BIT,
		ALL_BARRIER_BITS = GL_ALL_BARRIER_BITS,
	};

	class OpenGL {
	private:
	public:
		
		std::vector<bool> keys;
		std::vector<bool> bKeys;
		double mouseLastX, mouseLastY;
		double mouseCurrentX, mouseCurrentY;
		double scrollLast, scrollCurrent;
		int width, height;
		int backupWinX, backupWinY;
		int backupWidth, backupHeight;
		GLFWwindow* window;
		
	public:
		
		struct ErrorStruct {
			int code;
			const char* msg;
			const char* file;
			int line;
		};
		
		void FaceCulling(bool showFront=true, bool showBack=true);
		
		void PushCustomError(ErrorStruct err);
		int StackError(int line, const char* file);
		void ClearErrors();
		ErrorStruct PopError();
		ErrorStruct GetLastError();
		std::vector<ErrorStruct>& GetErrors();
		
		void PrintError(ErrorStruct err);
		void PrintLastError();
		void PrintErrors();
		
		void SetFullscreen(bool fullscreen);
		bool IsFullscreen() const;
		
		void SwapInput();
		bool IsKeyDown(const int id) const;
		bool IsKeyUp(const int id) const;
		bool WasKeyPressed(const int id) const;
		bool WasKeyReleased(const int id) const;
		double GetMouseDX() const;
		double GetMouseDY() const;
		double GetScrollDelta() const;
		double GetMouseX() const;
		double GetMouseY() const;
		double GetScroll() const;
		
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		
		int Init(const char* windowName, unsigned int width, unsigned int height,
				bool resizable, bool fullscreen, bool limitFrames = true,
				int majorOpenglVersion=4, int minorOpenglVersion=5);
		
		void SetKeyCallback(void (GLFWwindow*, int, int, int, int));
		void SetScrollCallback(void (GLFWwindow*, double, double));
		void SetMouseCallback(void (GLFWwindow*, double, double));
		
		void InitGraphic();
		void InitFrame();
		void SwapBuffer();
		
		void Destroy();
		
		OpenGL();
		~OpenGL();
		
	public:
		
		bool firstMouse;
		
	private:
		
		std::vector<ErrorStruct> errors;
	};
	
	void Flush();
	void Finish();
	void MemoryBarrier(GLbitfield barriers);

	extern OpenGL openGL;
}

#define GL_CHECK_PUSH_PRINT_ERROR {if(gl::openGL.StackError(__LINE__, __FILE__)){gl::openGL.PrintError(gl::openGL.GetLastError());}}
#define GL_CHECK_PUSH_ERROR gl::openGL.StackError(__LINE__, __FILE__)
#define GL_PUSH_CUSTOM_ERROR(code, msg) gl::openGL.PushCustomError({code, msg, __FILE__, __LINE__})
#define GL_PUSH_DEBUG_MESSAGE GL_PUSH_CUSTOM_ERROR(0, "Debug")

#endif

