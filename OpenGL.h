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

#ifndef OPEN_GL_ENGINE_H
#define OPEN_GL_ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "thirdparty/soil2/SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

void OpenGLKeyCallback(GLFWwindow* window, int key, int scancode, int action,
		int mode);
void OpenGLScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void OpenGLMouseCallback(GLFWwindow* window, double xPos, double yPos);
void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);
void OpenGLMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

namespace gl {
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
		UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV
	};
}

class OpenGL {
private:
public:
	
	std::vector<bool> keys;
	std::vector<bool> bKeys;
	double mouseLastX, mouseLastY;
	double mouseCurrentX, mouseCurrentY;
	double wheelDY;
	int width, height;
	int backupWinX, backupWinY;
	int backupWidth, backupHeight;
	GLFWwindow* window;
	
public:
	
	void SetFullscreen(bool fullscreen);
	bool IsFullscreen() const;
	
	void SwapInput();
	bool IsKeyDown(const int id) const;
	bool IsKeyUp(const int id) const;
	bool WasKeyPressed(const int id) const;
	bool WasKeyReleased(const int id) const;
	double GetMouseDX() const;
	double GetMouseDY() const;
	double GetWheelDY() const;
	
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	
	int Init(const char* windowName, unsigned int width, unsigned int height,
			bool resizable, bool fullscreen,
			int majorOpenglVersion=4, int minorOpenglVersion=6);
	
	void SetKeyCallback(void (GLFWwindow*, int, int, int, int));
	void SetScrollCallback(void (GLFWwindow*, double, double));
	void SetMouseCallback(void (GLFWwindow*, double, double));
	
	void InitGraphic();
	void InitFrame();
	void SwapBuffer();
	
	void Destroy();
	
	OpenGL();
	~OpenGL();
};

extern "C" OpenGL openGL;

#endif

