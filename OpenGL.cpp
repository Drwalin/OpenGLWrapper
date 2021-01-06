
#ifndef OPEN_GL_ENGINE_CPP
#define OPEN_GL_ENGINE_CPP

#include "OpenGL.h"

#include <cstdio>

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
		unsigned int height, bool resizable, bool fullscreen) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, resizable);
	window = glfwCreateWindow(width, height, windowName,
			fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if(window == NULL) {
		printf("\n Failed to create GLFW window! ");
		return 1;
	}
	glfwGetFramebufferSize(window, (int*)&(this->width), (int*)&(this->height));
	
	
	glfwSetKeyCallback(window, OpenGLKeyCallback);
	glfwSetCursorPosCallback(window, OpenGLMouseCallback);
	glfwSetScrollCallback(window, OpenGLScrollCallback);
	glfwSetWindowSizeCallback(window, OpenGLWindowResizeCallback);
	
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if(GLEW_OK != glewInit()) {
	    printf("\n Failed to initialize GLEW! ");
	    return 2;
	}
	return 0;
}

void OpenGL::InitGraphic() {
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
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
		}
		else if(GLFW_RELEASE == action)
		{
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

#endif

