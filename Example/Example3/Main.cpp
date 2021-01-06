
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Camera.cpp"


void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow * window, double xPos, double yPos);
void DoMovement();

#include <OpenGL.h>
#include <Shader.h>
#include <Texture.h>
#include <VAO.h>
#include <VBO.h>


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

#include <cstdio>

glm::vec2 scale(1,1), offset(0,0);

int main() {
	openGL.Init("Window test name 311", 800, 600, true, false);
	openGL.InitGraphic();
	
	glfwSetKeyCallback(openGL.window, KeyCallback);
	glfwSetCursorPosCallback(openGL.window, MouseCallback);
	glfwSetScrollCallback(openGL.window, ScrollCallback);
	
	Shader shaderLine;
	shaderLine.Load("vert.line.c", NULL, "frag.line.c");
	
	VBO vbo(2*sizeof(float), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	
	auto buf = vbo.Buffer<Atr<glm::vec2, 1>>();
	int i=0;
	float t=0;
	for(; t<(2.0*3.141592f); t+=0.01, ++i) {
		//buf.At<0>(i) = glm::vec2(t, sin(t));
		glm::vec2 p(sin(t), cos(t));
		buf.At<0>(i, 0) = p;
	}
	vbo.Generate();
	
	VAO vao(GL_LINE_LOOP);
	vao.SetAttribPointer(vbo, shaderLine.GetAttributeLocation("position"),
			2, GL_FLOAT, false, 0);
	
	unsigned scaleLoc =    shaderLine.GetUniformLocation("scale");
	unsigned offsetLoc =   shaderLine.GetUniformLocation("offset");
	unsigned colorLoc =    shaderLine.GetUniformLocation("color");
	unsigned viewportloc = shaderLine.GetUniformLocation("viewport_size");
	
	while(!glfwWindowShouldClose(openGL.window)) {
		/*
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		*/
		glfwPollEvents();
		
		openGL.InitFrame();
		
		shaderLine.Use();
		glm::vec4 color(0.9, 0.4, 0.1, 1.0);
		
		
		shaderLine.SetVec2(scaleLoc, scale);
		shaderLine.SetVec2(offsetLoc, offset);
		shaderLine.SetVec4(colorLoc, color);
		shaderLine.SetVec2(viewportloc, glm::vec2(openGL.width, openGL.height));
		
		vao.Draw();
		
		openGL.SwapBuffer();
	}
	
	return EXIT_SUCCESS;
}

void KeyCallback(GLFWwindow * window, int key, int scancode, int action,
		int mode) {
	if(GLFW_KEY_ESCAPE == key && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if(key >= 0 && key <= 1024) {
		if(GLFW_PRESS == action) {
			if(key == GLFW_KEY_P)
				openGL.SetFullscreen(!openGL.IsFullscreen());
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



