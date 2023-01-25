
#include <cstdio>
#define DEBUG(x) 
//printf("\n %i",(int)x);

#include <openglwrapper/OpenGL.h>
#include <openglwrapper/Shader.h>
#include <openglwrapper/Texture.h>
#include <openglwrapper/VAO.h>
#include <openglwrapper/VBO.h>

#include "../Camera.hpp"

namespace Texture {

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

int main() {
	gl::openGL.Init("Window test name 311", 800, 600, true, false);
	GL_CHECK_PUSH_ERROR;
    gl::openGL.InitGraphic();
	GL_CHECK_PUSH_ERROR;
    
    glfwSetKeyCallback(gl::openGL.window, KeyCallback);
	GL_CHECK_PUSH_ERROR;
    glfwSetCursorPosCallback(gl::openGL.window, MouseCallback);
	GL_CHECK_PUSH_ERROR;
    glfwSetScrollCallback(gl::openGL.window, ScrollCallback);
	GL_CHECK_PUSH_ERROR;
    
    gl::Shader ourShader;
	GL_CHECK_PUSH_ERROR;
	ourShader.Load(
			"../samples/Texture/vertex.glsl",
			NULL,
			"../samples/Texture/fragment.glsl");
	GL_CHECK_PUSH_ERROR;
    
    
    gl::VBO vbo(3*sizeof(float)+2*sizeof(uint16_t)+4*sizeof(uint8_t), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	GL_CHECK_PUSH_ERROR;
    auto buf = vbo.Buffer<gl::Atr<float, 3>, gl::Atr<short,2>, gl::Atr<uint8_t,4>>();
    for(int i = 0; i < 4; ++i) {
		buf.At<0>(i, 0) = (i>>0)&1;
		buf.At<0>(i, 1) = (i>>2)&1;
		buf.At<0>(i, 2) = (i>>1)&1;
		buf.At<1>(i, 0) = ((i>>0)&1)*(65535);
		buf.At<1>(i, 1) = ((i>>1)&1)*(65535);
		
		buf.At<2>(i, 0) = 
		buf.At<2>(i, 1) = 
		buf.At<2>(i, 2) = 
		buf.At<2>(i, 3) = 255;
	}
    vbo.Generate();
	GL_CHECK_PUSH_ERROR;
    
    gl::VAO vao(gl::TRIANGLE_STRIP);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3,
			gl::FLOAT, false, 0);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("uv"), 2,
			gl::UNSIGNED_SHORT, true, 12);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4,
			gl::UNSIGNED_BYTE, true, 16);
	GL_CHECK_PUSH_ERROR;
    
    
    
	gl::Texture texture;
	GL_CHECK_PUSH_ERROR;
    texture.Load("../samples/Simple/image.jpg", false, 4);
	GL_CHECK_PUSH_ERROR;
    
	printf("tex loc: %i\n", ourShader.GetUniformLocation("tex"));
    
// 	glPointSize(3.0f);
// 	glLineWidth(3.0f);
// 	glEnable(GL_DITHER);
	
	
	
	uint8_t* p = new uint8_t[1024*1024*256];
	texture.Bind();
	glGetTexImage(gl::TEXTURE_2D,
			0,
			gl::RGBA,
			GL_UNSIGNED_BYTE,
			p);
	
	printf(" color: %X\n", ((uint32_t*)p)[0]);
	printf(" color: %X\n", ((uint32_t*)p)[texture.GetWidth()/2]);
	
	
	GLint modelLoc = ourShader.GetUniformLocation("model");
	GLint viewLoc = ourShader.GetUniformLocation("view");
	GLint projLoc = ourShader.GetUniformLocation("projection");
	
    camera.ProcessMouseMovement(150, -200);
	
    while(!glfwWindowShouldClose(gl::openGL.window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents();
        DoMovement();
        
        gl::openGL.InitFrame();
        
	GL_CHECK_PUSH_ERROR;
        
        
        
        ourShader.Use();
	GL_CHECK_PUSH_ERROR;
		ourShader.SetTexture(ourShader.GetUniformLocation("tex"), &texture, 7);
	GL_CHECK_PUSH_ERROR;
        
        glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);
        
        // Create transformations
//         glm::mat4 model;
        glm::mat4 view;

        view = camera.getViewMatrix();
        
        ourShader.SetMat4(viewLoc, view);
	GL_CHECK_PUSH_ERROR;
        ourShader.SetMat4(projLoc, projection);
	GL_CHECK_PUSH_ERROR;
        
		glm::mat4 model(1.0f);
		model = glm::scale(model, glm::vec3(10));
		model = glm::translate(model,
				glm::vec3(0.0f,-1.0f,0.0f));
		ourShader.SetMat4(modelLoc, model);
		vao.Draw();//0, 36);
	GL_CHECK_PUSH_ERROR;
        
        gl::openGL.SwapBuffer();
	GL_CHECK_PUSH_ERROR;
		gl::openGL.PrintErrors();
		gl::openGL.ClearErrors();
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

}


