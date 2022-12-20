
#define DEBUG(x) 
//printf("\n %i",(int)x);

#include "../../OpenGL.h"
#include "../../Shader.h"
#include "../../Texture.h"
#include "../../VAO.h"
#include "../../VBO.h"

#include "../Camera.hpp"

// Window dimensions
//const GLuint WIDTH = 800, HEIGHT = 600;
//int SCREEN_WIDTH, SCREEN_HEIGHT;
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


#include <cstdio>

int main() {
	gl::openGL.Init("Window test name 311", 800, 600, true, false);
    gl::openGL.InitGraphic();
    
    glfwSetKeyCallback(gl::openGL.window, KeyCallback);
    glfwSetCursorPosCallback(gl::openGL.window, MouseCallback);
    glfwSetScrollCallback(gl::openGL.window, ScrollCallback);
    
    gl::Shader ourShader;
	ourShader.Load(
			"../Example/ExampleBasicShaders/vertex.glsl",
			"../Example/ExampleBasicShaders/geometry.glsl",
			"../Example/ExampleBasicShaders/fragment.glsl");
    
    
    gl::VBO vbo(3*sizeof(float), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
    auto buf = vbo.Buffer<gl::Atr<glm::vec3, 1>>();
    for(int i = 0; i < 8; ++i)
        buf.At<0>(i) = glm::vec3(i, i/2.f, i/3.f);
    vbo.Generate();
    
    gl::VAO vao(gl::POINTS);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3,
			gl::FLOAT, false, 0);
    
    
    
	gl::Texture texture;
    texture.Load("../Example/ExampleBasicShaders/image.jpg", false, 4);
    
    ourShader.SetTexture(ourShader.GetUniformLocation("ourTexture1"), &texture,
			0);
    
	glPointSize(3.0f);
	glLineWidth(3.0f);
	glEnable(GL_DITHER);
	
	
	GLint modelLoc = ourShader.GetUniformLocation("model");
	GLint viewLoc = ourShader.GetUniformLocation("view");
	GLint projLoc = ourShader.GetUniformLocation("projection");
	
    while(!glfwWindowShouldClose(gl::openGL.window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents();
        DoMovement();
        
        gl::openGL.InitFrame();
        
        
        
        
        ourShader.Use();
        
        glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);
        
        // Create transformations
//         glm::mat4 model;
        glm::mat4 view;

        view = camera.getViewMatrix();
        
        ourShader.SetMat4(viewLoc, view);
        ourShader.SetMat4(projLoc, projection);
        
        for(int j = 1; j < 101; ++j) {
	        for(GLuint i = 1; i < 11; ++i) {
	            glm::mat4 model(1.0f);
                model = glm::scale(model, glm::vec3(10));
                model = glm::translate(model,
						glm::vec3(0.0f,0.0f,0.0f+float((j*i)<<1)));
                ourShader.SetMat4(modelLoc, model);
                vao.SetInstances(100*1);
                vao.Draw();//0, 36);
	        }
	    }
        
        gl::openGL.SwapBuffer();
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
	
    
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

void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode) {
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



