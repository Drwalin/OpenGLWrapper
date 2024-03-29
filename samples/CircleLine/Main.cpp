
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Camera.hpp"

#include "../../include/openglwrapper/OpenGL.hpp"
#include "../../include/openglwrapper/Shader.hpp"
#include "../../include/openglwrapper/Texture.hpp"
#include "../../include/openglwrapper/VAO.hpp"
#include "../../include/openglwrapper/VBO.hpp"
#include "../../include/openglwrapper/BufferAccessor.hpp"

namespace CircleLine {

void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow * window, double xPos, double yPos);
void DoMovement();

Camera camera(glm::vec3(0.0f,0.0f, 0.0f));
GLfloat lastX = 0.0;//WIDTH/2.0;
GLfloat lastY = 0.0;//WIDTH/2.0;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

float randf(float min, float max) {
	return (max-min)*(rand()) / (float)RAND_MAX + min;
}

glm::vec2 scale(1,1), offset(0,0);

int main() {
	gl::openGL.Init("Window test name 311", 800, 600, true, false);
	gl::openGL.InitGraphic();
	
	glfwSetKeyCallback(gl::openGL.window, KeyCallback);
	glfwSetCursorPosCallback(gl::openGL.window, MouseCallback);
	glfwSetScrollCallback(gl::openGL.window, ScrollCallback);
	
	gl::Shader shaderLine;
	shaderLine.Load(
			"../samples/CircleLine/vertex.glsl",
			"",
			"../samples/CircleLine/fragment.glsl");
	
	gl::VBO vbo(2*sizeof(float), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	vbo.Init();
	
	std::vector<uint8_t> Vbo;
	gl::BufferAccessor::BufferRef<gl::Atr<glm::vec2, 1>> buf(&vbo, Vbo);
	int i=0;
	float t=0;
	for(; t<(2.0*3.141592f); t+=0.01, ++i) {
		//buf.At<0>(i) = glm::vec2(t, sin(t));
		glm::vec2 p(sin(t), cos(t));
		buf.At<0>(i, 0) = p;
	}
	vbo.Generate(Vbo);
	
	gl::VAO vao(gl::LINE_LOOP);
	vao.Init();
	vao.SetAttribPointer(vbo, shaderLine.GetAttributeLocation("position"),
			2, gl::FLOAT, false, 0);
	
	unsigned scaleLoc =    shaderLine.GetUniformLocation("scale");
	unsigned offsetLoc =   shaderLine.GetUniformLocation("offset");
	unsigned colorLoc =    shaderLine.GetUniformLocation("color");
	unsigned viewportloc = shaderLine.GetUniformLocation("viewport_size");
	
	while(!glfwWindowShouldClose(gl::openGL.window)) {
		/*
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		*/
		glfwPollEvents();
		
		gl::openGL.InitFrame();
		
		shaderLine.Use();
		glm::vec4 color(0.9, 0.4, 0.1, 1.0);
		
		
		shaderLine.SetVec2(scaleLoc, scale);
		shaderLine.SetVec2(offsetLoc, offset);
		shaderLine.SetVec4(colorLoc, color);
		shaderLine.SetVec2(viewportloc, glm::vec2(gl::openGL.width, gl::openGL.height));
		
		vao.Draw();
		
		gl::openGL.SwapBuffer();
	}
	
	return EXIT_SUCCESS;
}

void KeyCallback(GLFWwindow * window, int key, int scancode, int action,
		int mode) {
	if(GLFW_KEY_ESCAPE == key && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if(key >= 0 && key <= 1024) {
		if(GLFW_PRESS == action) {
			if(key == GLFW_KEY_P)
				gl::openGL.SetFullscreen(!gl::openGL.IsFullscreen());
			keys[key] = true;
		} else if(GLFW_RELEASE == action) {
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow * window, double xPos, double yPos) {
	if(firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	
	GLfloat xOfsset = xPos - lastX;
	GLfloat yOfsset = lastY - yPos;
	
	lastX = xPos;
	lastY = yPos;
	
	if(keys[GLFW_KEY_SPACE])
		offset += glm::vec2(xOfsset, yOfsset) * 0.001f / scale;
}

void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(yOffset);
	//	printf("\n scroll: %f %f", xOffset, yOffset);
	offset /= scale;
	scale *= pow(1.3, yOffset);
	offset *= scale;
}

}

