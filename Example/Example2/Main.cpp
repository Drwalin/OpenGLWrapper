
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

#define DEBUG(x) 
//printf("\n %i",(int)x);



// Window dimensions
//const GLuint WIDTH = 800, HEIGHT = 600;
//int SCREEN_WIDTH, SCREEN_HEIGHT;
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

int main() {
	openGL.Init("Window test name 311", 800, 600, true, false);
	openGL.InitGraphic();
	
	glfwSetKeyCallback(openGL.window, KeyCallback);
	glfwSetCursorPosCallback(openGL.window, MouseCallback);
	glfwSetScrollCallback(openGL.window, ScrollCallback);
	
	Shader ourShader;
	ourShader.Load("vs.core.c", "gs.core.c", "fs.core.c");
	
	VBO vbo(11*sizeof(float), GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	auto buf = vbo.Buffer<Atr<glm::vec3, 2>, Atr<float, 1>, Atr<glm::vec4, 1>>();
	for(int i = 1000; i >= 0; --i) {
		auto rnd = std::bind(randf, -0.1, 0.1);
		buf.At<0>(i, 0) = glm::vec3(rnd(), rnd(), rnd())*0.4f;
		buf.At<0>(i, 1) = glm::vec3(rnd()*10, 20*rnd()+25, 3+rnd()*10);
		buf.At<1>(i) = randf(2.3f, 4.1f);
		float b = randf(0.7, 1);
		buf.At<2>(i) = glm::vec4(
				randf(0, 0.4) * b,
				randf(0, 0.6),
				b,
				randf(0, 1)
				);
	}
	vbo.Generate();
	
	VAO vao(GL_POINTS);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3,
			GL_FLOAT, false, 0);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("startVelocity"), 3,
			GL_FLOAT, false, 12);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("lifeTime"), 1,
			GL_FLOAT, false, 24);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4,
			GL_FLOAT, false, 28);
	
	unsigned timeUniformLocation = ourShader.GetUniformLocation("time");
	unsigned accelerationLoc = ourShader.GetUniformLocation("acceleration");
	
	Texture texture;
	texture.Load("image.jpg", GL_REPEAT, GL_NEAREST, false);
	
	ourShader.SetTexture(ourShader.GetUniformLocation("ourTexture1"), &texture,
			0);
	
	while(!glfwWindowShouldClose(openGL.window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		glfwPollEvents();
		DoMovement();
		
		openGL.InitFrame();
		
		
		
		
		ourShader.Use();
		
		glm::mat4 projection = glm::perspective(45.0f,
				(float)openGL.GetWidth()/(float)openGL.GetHeight(), 0.1f,
				10000.0f);
		
		// Create transformations
		glm::mat4 model;
		glm::mat4 view;

		view = camera.getViewMatrix();
		
		GLint modelLoc = ourShader.GetUniformLocation("model");
		GLint viewLoc = ourShader.GetUniformLocation("view");
		GLint projLoc = ourShader.GetUniformLocation("projection");
		
		ourShader.SetMat4(viewLoc, view);
		ourShader.SetMat4(projLoc, projection);
		ourShader.SetFloat(timeUniformLocation, (float)(clock()) * 0.00041f);
		ourShader.SetVec3(accelerationLoc, glm::vec3(0, -10, 0));
		
		for(int j = 0; j < 1; ++j) {
			for(GLuint i = 10; i < 11; ++i) {
				glm::mat4 model(1.0f);
				model = glm::scale(model, glm::vec3(10));
				model = glm::translate(model,
						glm::vec3(0.0f,0.0f,0.0f+float((j*i)<<1)));
				ourShader.SetMat4(modelLoc, model);
				//vao.SetInstances(1000*1000);
				//glDepthMask(false);
				vao.Draw();//0, 36);
				//glDepthMask(true);
			}
		}
		
		openGL.SwapBuffer();
	}
	
	return EXIT_SUCCESS;
}

void DoMovement() {
	if(keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if(keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if(keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if(keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if(keys[GLFW_KEY_Q]) {
		camera.Up(-1, deltaTime);
	}
	if(keys[GLFW_KEY_E]) {
		camera.Up(1, deltaTime);
	}
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
	
	camera.ProcessMouseMovement(xOfsset, yOfsset);
}

void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(yOffset);
}



