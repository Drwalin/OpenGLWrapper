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
#include <SOIL2\SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

void OpenGLKeyCallback(GLFWwindow* window, int key, int scancode, int action,
		int mode);
void OpenGLScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void OpenGLMouseCallback(GLFWwindow* window, double xPos, double yPos);
void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height);
void OpenGLMouseButtonCallback(GLFWwindow* window, int button, int action,
		int mods)

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
			bool resizable, bool fullscreen);
	
	void InitGraphic();
	void InitFrame();
	void SwapBuffer();
	
	void Destroy();
	
	OpenGL();
	~OpenGL();
};

extern "C" OpenGL openGL;

#endif

