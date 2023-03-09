
#ifndef DEFAULTS_CAMERA_AND_OTHER_SAMPLES_STUFF_HPP
#define DEFAULTS_CAMERA_AND_OTHER_SAMPLES_STUFF_HPP

#include <cstdio>
#include <ctime>

#include "../include/openglwrapper/OpenGL.hpp"
#include "../include/openglwrapper/Shader.hpp"
#include "../include/openglwrapper/Texture.hpp"
#include "../include/openglwrapper/VAO.hpp"
#include "../include/openglwrapper/VBO.hpp"

#include "Camera.hpp"

static void DoMovement();

static Camera camera(glm::vec3(0.0f,0.0f, 0.0f));

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

template<typename T=int>
static void DefaultsSetup() {
	srand(time(NULL));
	gl::openGL.Init("Window test name 311", 800, 600, true, false);
    gl::openGL.InitGraphic();
}

static bool WasPressed(int key) {
	gl::openGL.WasKeyPressed(key);
}

static void DefaultIterationStart() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	
	gl::openGL.SwapInput();

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
	if(gl::openGL.WasKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(gl::openGL.window, GL_TRUE);
	}
	float mult = 1;
	if(gl::openGL.IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
		mult = 10;
	}
    if(gl::openGL.IsKeyDown(GLFW_KEY_W)) {
        camera.ProcessKeyboard(FORWARD, deltaTime*mult);
    }
    if(gl::openGL.IsKeyDown(GLFW_KEY_S)) {
        camera.ProcessKeyboard(BACKWARD, deltaTime*mult);
    }
    if(gl::openGL.IsKeyDown(GLFW_KEY_D)) {
        camera.ProcessKeyboard(RIGHT, deltaTime*mult);
    }
    if(gl::openGL.IsKeyDown(GLFW_KEY_A)) {
        camera.ProcessKeyboard(LEFT, deltaTime*mult);
    }
    if(gl::openGL.IsKeyDown(GLFW_KEY_Q)) {
        camera.Up(-1, deltaTime*mult);
    }
    if(gl::openGL.IsKeyDown(GLFW_KEY_E)) {
        camera.Up(1, deltaTime*mult);
    }
	{
		float y = 0, x=0;
		if(gl::openGL.IsKeyDown(GLFW_KEY_UP)) {
			y = 1;
		}
		if(gl::openGL.IsKeyDown(GLFW_KEY_DOWN)) {
			y = -1;
		}
		if(gl::openGL.IsKeyDown(GLFW_KEY_RIGHT)) {
			x = 1;
		}
		if(gl::openGL.IsKeyDown(GLFW_KEY_LEFT)) {
			x = -1;
		}
		camera.ProcessMouseMovement(x*deltaTime*100*mult, y*deltaTime*100*mult);
	}
	
	camera.ProcessMouseMovement(gl::openGL.GetMouseDX(),
			gl::openGL.GetMouseDY());
    camera.ProcessMouseScroll(gl::openGL.GetScrollDelta());
	printf(" mouse dy = %f\n", gl::openGL.GetMouseDY());
}

const inline void *const*const ptrs = new void*[]{
	(void*)DefaultIterationStart,
	(void*)DefaultIterationEnd,
	(void*)WasPressed,
};

#endif

