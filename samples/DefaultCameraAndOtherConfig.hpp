
#ifndef DEFAULTS_CAMERA_AND_OTHER_SAMPLES_STUFF_HPP
#define DEFAULTS_CAMERA_AND_OTHER_SAMPLES_STUFF_HPP

#include <cstdio>

#include <openglwrapper/OpenGL.h>
#include <openglwrapper/Shader.h>
#include <openglwrapper/Texture.h>
#include <openglwrapper/VAO.h>
#include <openglwrapper/VBO.h>

#include "Camera.hpp"

static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);
static void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset);
static void MouseCallback(GLFWwindow * window, double xPos, double yPos);
static void DoMovement();

static Camera camera(glm::vec3(0.0f,0.0f, 0.0f));
static float lastX = 0.0;//WIDTH/2.0;
static float lastY = 0.0;//WIDTH/2.0;
static bool keys[1024];
static bool firstMouse = true;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

template<typename T=int>
static void DefaultsSetup() {
	gl::openGL.Init("Window test name 311", 800, 600, true, false);
    gl::openGL.InitGraphic();
	
    glfwSetKeyCallback(gl::openGL.window, KeyCallback);
    glfwSetCursorPosCallback(gl::openGL.window, MouseCallback);
    glfwSetScrollCallback(gl::openGL.window, ScrollCallback);
}

static void DefaultIterationStart() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glfwPollEvents();
	DoMovement();

	gl::openGL.InitFrame();
}

static void DefaultIterationEnd() {
	gl::openGL.SwapBuffer();
	gl::openGL.PrintErrors();
	gl::openGL.ClearErrors();
}

static void DoMovement() {
	float mult = 1;
	if(keys[GLFW_KEY_LEFT_SHIFT]) {
		mult = 10;
	}
    if(keys[GLFW_KEY_W]) {
        camera.ProcessKeyboard(FORWARD, deltaTime*mult);
    }
    if(keys[GLFW_KEY_S]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime*mult);
    }
    if(keys[GLFW_KEY_D]) {
        camera.ProcessKeyboard(RIGHT, deltaTime*mult);
    }
    if(keys[GLFW_KEY_A]) {
        camera.ProcessKeyboard(LEFT, deltaTime*mult);
    }
    if(keys[GLFW_KEY_Q]) {
        camera.Up(-1, deltaTime*mult);
    }
    if(keys[GLFW_KEY_E]) {
        camera.Up(1, deltaTime*mult);
    }
	{
		float y = 0, x=0;
		if(keys[GLFW_KEY_UP]) {
			y = 1;
		}
		if(keys[GLFW_KEY_DOWN]) {
			y = -1;
		}
		if(keys[GLFW_KEY_RIGHT]) {
			x = 1;
		}
		if(keys[GLFW_KEY_LEFT]) {
			x = -1;
		}
		camera.ProcessMouseMovement(x*deltaTime*100*mult, y*deltaTime*100*mult);
	}
}

static void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode) {
    if(GLFW_KEY_ESCAPE == key && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key >= 0 && key <= 1024) {
        if(GLFW_PRESS == action) {
            keys[key] = true;
        } else if(GLFW_RELEASE == action) {
            keys[key] = false;
        }
    }
}

static void MouseCallback(GLFWwindow * window, double xPos, double yPos) {
    if(firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    
    lastX = xPos;
    lastY = yPos;
	
	camera.ProcessMouseMovement(xOffset, yOffset);
}

static void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset) {
    camera.ProcessMouseScroll(yOffset);
}

const inline void *const*const ptrs = new void*[]{
	(void*)KeyCallback,
	(void*)MouseCallback,
	(void*)ScrollCallback,
	(void*)DefaultIterationStart,
	(void*)DefaultIterationEnd,
};

#endif

