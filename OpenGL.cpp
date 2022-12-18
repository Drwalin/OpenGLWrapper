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

#ifndef OPEN_GL_ENGINE_CPP
#define OPEN_GL_ENGINE_CPP

#include "OpenGL.h"

#include <cstdio>

#include <cstdio>

namespace gl {

OpenGL openGL;

void OpenGL::SetFullscreen(bool fullscreen) {
	if(fullscreen == IsFullscreen())
		return;
	if(fullscreen) {
		glfwGetWindowPos(window, &backupWinX, &backupWinY);
		glfwGetWindowSize(window, &backupWidth, &backupHeight);
		
		auto monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode * mode = glfwGetVideoMode(monitor);
		
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height,
				0);
	} else {
		glfwSetWindowMonitor(window, nullptr, backupWinX, backupWinY,
				backupWidth, backupHeight, 0);
	}
}

bool OpenGL::IsFullscreen() const {
	return glfwGetWindowMonitor(window) != NULL;
}

void OpenGL::SwapInput() {
	bKeys = keys;
	for(auto it : keys)
		it = false;
	mouseLastX = mouseCurrentX;
	mouseLastY = mouseCurrentY;
}

bool OpenGL::IsKeyDown(const int id) const {
	if(id<0 || id>=keys.size())
		return false;
	return keys[id];
}

bool OpenGL::IsKeyUp(const int id) const {
	if(id<0 || id>=keys.size())
		return false;
	return !keys[id];
}

bool OpenGL::WasKeyPressed(const int id) const {
	if(id<0 || id>=keys.size())
		return false;
	return keys[id] && !bKeys[id];
}

bool OpenGL::WasKeyReleased(const int id) const {
	if(id<0 || id>=keys.size())
		return false;
	return !keys[id] && bKeys[id];
}

double OpenGL::GetMouseDX() const {
	return mouseCurrentX - mouseLastX;
}

double OpenGL::GetMouseDY() const {
	return mouseCurrentY - mouseLastY;
}

double OpenGL::GetWheelDY() const {
	return wheelDY;
}

unsigned int OpenGL::GetWidth() const {
	return width;
}

unsigned int OpenGL::GetHeight() const {
	return height;
}

int OpenGL::Init(const char* windowName, unsigned int width,
		unsigned int height, bool resizable, bool fullscreen,
		int majorOpenglVersion, int minorOpenglVersion) {
	GL_CHECK_PUSH_ERROR;
	glfwInit();
	GL_CHECK_PUSH_ERROR;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorOpenglVersion);
	GL_CHECK_PUSH_ERROR;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorOpenglVersion);
	GL_CHECK_PUSH_ERROR;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GL_CHECK_PUSH_ERROR;
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GL_CHECK_PUSH_ERROR;
    glfwWindowHint(GLFW_RESIZABLE, resizable);
	GL_CHECK_PUSH_ERROR;
	window = glfwCreateWindow(width, height, windowName,
			fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	GL_CHECK_PUSH_ERROR;
	if(window == NULL) {
		printf("\n Failed to create GLFW window! ");
		return 1;
	}
	GL_CHECK_PUSH_ERROR;
	glfwGetFramebufferSize(window, (int*)&(this->width), (int*)&(this->height));
	GL_CHECK_PUSH_ERROR;
	
	
	glfwSetKeyCallback(window, OpenGLKeyCallback);
	GL_CHECK_PUSH_ERROR;
	glfwSetCursorPosCallback(window, OpenGLMouseCallback);
	GL_CHECK_PUSH_ERROR;
	glfwSetScrollCallback(window, OpenGLScrollCallback);
	GL_CHECK_PUSH_ERROR;
	glfwSetWindowSizeCallback(window, OpenGLWindowResizeCallback);
	GL_CHECK_PUSH_ERROR;
	glfwSetMouseButtonCallback(window, OpenGLMouseButtonCallback);
	GL_CHECK_PUSH_ERROR;
	
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	GL_CHECK_PUSH_ERROR;
	glfwMakeContextCurrent(window);
	GL_CHECK_PUSH_ERROR;
	glewExperimental = GL_TRUE;
	if(GLEW_OK != glewInit()) {
	GL_CHECK_PUSH_ERROR;
	    printf("\n Failed to initialize GLEW! ");
	    return 2;
	}
	GL_CHECK_PUSH_ERROR;
	return 0;
}



void OpenGL::SetKeyCallback(void (callback)(GLFWwindow*, int, int, int, int)) {
    glfwSetKeyCallback(window, callback);
}

void OpenGL::SetScrollCallback(void (callback)(GLFWwindow*, double, double)) {
    glfwSetScrollCallback(window, callback);
}

void OpenGL::SetMouseCallback(void (callback)(GLFWwindow*, double, double)) {
    glfwSetCursorPosCallback(window, callback);
}



void OpenGL::InitGraphic() {
	GL_CHECK_PUSH_ERROR;
	glViewport(0, 0, width, height);
	GL_CHECK_PUSH_ERROR;
	glEnable(GL_DEPTH_TEST);
	GL_CHECK_PUSH_ERROR;
	glEnable(GL_BLEND);
	GL_CHECK_PUSH_ERROR;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL_CHECK_PUSH_ERROR;
	glDepthFunc(GL_LESS);
	GL_CHECK_PUSH_ERROR;
	
// 	GL_CHECK_PUSH_ERROR;
// 	glEnable(GL_ALPHA_TEST);
// 	GL_CHECK_PUSH_ERROR;
// 	glAlphaFunc(GL_GREATER, 0.5);
// 	GL_CHECK_PUSH_ERROR;
}

void OpenGL::InitFrame() {
	//glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
}

void OpenGL::SwapBuffer() {
	glFlush();
	glfwSwapBuffers(window);
}

void OpenGL::Destroy() {
	glfwDestroyWindow(window);
	window = NULL;
	width = height = 0;
}

OpenGL::OpenGL() {
	mouseLastX = mouseLastY = mouseCurrentX = mouseCurrentY = wheelDY = 0.0;
	keys.resize(1024);
	for(auto it : keys)
		it = false;
	SwapInput();
}

OpenGL::~OpenGL() {
	Destroy();
	glfwTerminate();
}

void OpenGLKeyCallback(GLFWwindow* window, int key, int scancode, int action,
		int mode) {
    if(key >= 0 && key <= 1024) {
		if(GLFW_PRESS == action) {
			openGL.keys[key] = true;
		} else if(GLFW_RELEASE == action) {
			openGL.keys[key] = false;
		}
	}
}

void OpenGLScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	openGL.wheelDY += yOffset;
}

void OpenGLMouseCallback(GLFWwindow* window, double xPos, double yPos) {
	static bool firstMouse = true;
	if(firstMouse) {
		openGL.mouseLastX = openGL.mouseCurrentX;
		openGL.mouseLastY = openGL.mouseCurrentY;
		firstMouse = false;
	}
	openGL.mouseCurrentX = xPos;
	openGL.mouseCurrentY = yPos;
}

void OpenGLWindowResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	openGL.width = width;
	openGL.height = height;
}

void OpenGLMouseButtonCallback(GLFWwindow* window, int button,
		int action, int mods) {
	if(button==GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS) {
		int a = 0;
		a *= 3;
	}
}



int OpenGL::StackError(int line, const char* file) {
	ErrorStruct err;
	err.code = glGetError();
	if(err.code != GL_NO_ERROR) {
		err.msg = (const char*)gluErrorString(err.code);
		err.line = line;
		err.file = file;
		errors.emplace_back(err);
	}
	return err.code;
}

int OpenGL::PrintError(ErrorStruct err) {
	fprintf(stderr, "%s:%i -> OpenGL error [%i]: %s\n", err.file, err.line,
			err.code, err.msg);
	fflush(stderr);
}

void OpenGL::PrintErrors() {
	for(int i=0; i<errors.size(); ++i) {
		PrintError(errors[i]);
	}
}

void OpenGL::PrintLastError() {
	PrintError(errors.back());
}

OpenGL::ErrorStruct OpenGL::GetLastError() {
	if(errors.size() > 0) {
		return errors.back();
	}
	return ErrorStruct{GL_NO_ERROR};
}

OpenGL::ErrorStruct OpenGL::PopError() {
	auto err = GetLastError();
	if(errors.size() > 0)
		errors.pop_back();
	return err;
}

std::vector<OpenGL::ErrorStruct>& OpenGL::GetErrors() {
	return errors;
}

void OpenGL::ClearErrors() {
	errors.clear();
}



GLenum errorCheck(int line, const char* file) {
	GLenum code = glGetError();
	const GLubyte* string;
	if(code != GL_NO_ERROR) {
		string = gluErrorString(code);
		fprintf(stderr, "%s:%i -> OpenGL error [%i]: %s\n", file, line, code, string);
		exit(311);
	}
	return code;
}

}

#endif

