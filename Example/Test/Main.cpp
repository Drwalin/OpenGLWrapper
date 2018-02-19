#include <cstdio>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "C:\programowanie\cpp\OpenGL\SpartanJ-soil2-54073b423037\src\SOIL2\SOIL2.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Camera.cpp"

#define DEBUG(x) 
//printf( "\n %i", (int)x );



// Window dimensions
//const GLuint WIDTH = 800, HEIGHT = 600;
//int SCREEN_WIDTH, SCREEN_HEIGHT;
void KeyCallback( GLFWwindow * window, int key, int scancode, int action, int mode );
void ScrollCallback (GLFWwindow * window, double xOffset, double yOffset);
void MouseCallback (GLFWwindow * window, double xPos, double yPos);
void DoMovement();

#include "../../OpenGL.cpp"
#include "../../Shader.cpp"
#include "../../Texture.cpp"
#include "../../VAO.cpp"


Camera camera(glm::vec3 (0.0f,0.0f, 0.0f));
GLfloat lastX = 0.0;//WIDTH/2.0;
GLfloat lastY = 0.0;//WIDTH/2.0;
bool keys[1024];
bool firstMouse = true;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;





// The MAIN function, from here we start the application and run the game loop
int main()
{
    openGL.Init( "Window test name 311", 800, 600, false, false );
    openGL.InitGraphic();
    
    glfwSetKeyCallback( openGL.window, KeyCallback );
    glfwSetCursorPosCallback( openGL.window, MouseCallback );
    glfwSetScrollCallback( openGL.window, ScrollCallback );
    
    Shader ourShader;
	ourShader.Load( "../GeometryShader/core.vs", "../GeometryShader/core.gs", "../GeometryShader/core.fs" );
    
    GLfloat vertices[] =
	{
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    VAO vao;
    vao.SetVertexSize( 5*sizeof(float) );
    
    for( int i = 0; i < 36; ++i )
    {
    	vao.AddVertex( vertices+(i*5*sizeof(float)) );
    }
    
    glm::vec3 cubePositions[]
    {
        glm::vec3 (0.0f,0.0f, 0.0f),
        glm::vec3 (2.0f,5.0f, -15.0f),
        glm::vec3 (-1.5f,-2.2f, -2.5f),
        glm::vec3 (-3.8f,-2.0f, -12.3f),
        glm::vec3 (2.4f,-0.4f, -3.5f),
        glm::vec3 (-1.7f,3.0f, -7.5f),
        glm::vec3 (1.3f,-2.0f, -2.5f),
        glm::vec3 (1.5f,2.0f, -2.5f),
        glm::vec3 (-1.5f,0.2f, -1.5f),
        glm::vec3 (-1.3f,1.0f, -1.5f)
    };
    
    vao.Generate();
    vao.ClearVertices();
	vao.SetAttribPointer( ourShader.GetAttributeLocation( "position" ), 3, GL_FLOAT, false, 0 );
	
	Texture texture;
    texture.Load( "resources/imafges/image1.jpg", GL_REPEAT, GL_LINEAR, false );
    
    while ( !glfwWindowShouldClose( openGL.window ) )
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents( );
        DoMovement();
        
        openGL.InitFrame();
        
        
		glActiveTexture( GL_TEXTURE0 );
        texture.Bind();
        ourShader.SetInt( ourShader.GetUniformLocation( "ourTexture1" ), 0 );
        
        
        ourShader.Use();
        
        glm::mat4 projection = glm::perspective( 45.0f, ( GLfloat )openGL.GetWidth() / ( GLfloat )openGL.GetHeight(), 0.001f, 10000.0f );
        
        // Create transformations
        glm::mat4 model;
        glm::mat4 view;

        view = camera.getViewMatrix();
        
        GLint modelLoc = ourShader.GetUniformLocation( "model" );
        GLint viewLoc = ourShader.GetUniformLocation( "view" );
        GLint projLoc = ourShader.GetUniformLocation( "projection" );
        
        ourShader.SetMat4( viewLoc, view );
        ourShader.SetMat4( projLoc, projection );
        
        //printf( "\n Camera:  %f %f %f ", camera.position.x, camera.position.y, camera.position.z );
        
        for( int j = 0; j < 1; ++j )
        {
	        for( GLuint i = 10; i < 11; ++i )
	        {
	            glm::mat4 model(1.0f);
	            {
		            model = glm::translate( model, glm::vec3(0.0f,0.0f,0.0f+float((j*i)<<1)) );
        			ourShader.SetMat4( modelLoc, model );
	         		vao.Draw( GL_POINTS, 0, 1 );
		        }/*
		        else
		        {
		            model = glm::translate( model, cubePositions[i]+glm::vec3(float(j<<1),float(j<<1),float(j<<1)) );
	            	glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
	         		glDrawArrays( GL_TRIANGLES, 0, 36 );
	            }*/
	        }
	    }
        
        openGL.SwapBuffer();
    }
    
    return EXIT_SUCCESS;
}

void DoMovement()
{
    if( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
}

void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
    if( GLFW_KEY_ESCAPE == key && action == GLFW_PRESS )
    {
        
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key >= 0 && key <= 1024)
    {
        if(GLFW_PRESS == action)
        {
            keys[key] = true;
        }
        else if( GLFW_RELEASE == action)
        {
            keys[key] = false;
            
        }
    }
}

void MouseCallback(GLFWwindow * window, double xPos, double yPos )
{
    
    if( firstMouse )
    {
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

void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset)
{

    camera.ProcessMouseScroll(yOffset);
    

}



