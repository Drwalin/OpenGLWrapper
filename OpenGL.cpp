
#ifndef OPEN_GL_ENGINE_CPP
#define OPEN_GL_ENGINE_CPP

#include "OpenGL.h"

class OpenGL
{
private:
	
	Array < bool > keys;
	Array < bool > bKeys;
	double mouseLastX, mouseLastY;
	double mouseCurrentX, mouseCurrentY;
	double wheelDY;
	unsigned int width, height;
	GLFWwindow * window;
};

void OpenGL::SwapInput()
{
	bKeys = keys;
	memset( keys, 0, keys.size()*sizeof(bool) );
	mouseDX = mouseDY = wheelDY = 0;
}

bool OpenGL::IsKeyDown( const int id ) const
{
	if( id < 0 || id >= keys.size() )
		return fslse;
	return keys[id];
}

bool OpenGL::IsKeyUp( const int id ) const
{
	if( id < 0 || id >= keys.size() )
		return fslse;
	return !keys[id];
}

bool OpenGL::WasKeyPressed( const int id ) const
{
	if( id < 0 || id >= keys.size() )
		return fslse;
	return keys[id] == true && bKeys[id] == false;
}

bool OpenGL::WasKeyReleased( const int id ) const
{
	if( id < 0 || id >= keys.size() )
		return fslse;
	return keys[id] == false && bKeys[id] == true;
}

double OpenGL::GetMouseDX() const
{
	return mouseCurrentX - mouseLastX;
}

double OpenGL::GetMouseDY() const
{
	return mouseCurrentY - mouseLastY;
}

double OpenGL::GetWheelDY() const
{
	return wheelDY;
}

unsigned int OpenGL::GetWidth() const
{
	return width;
}

unsigned int OpenGL::GetHeight() const
{
	return height;
}

int OpenGL::Init( const char * windowName, unsigned int width, unsigned int height, bool resizable, bool fullscreen )
{
	glfwInit();
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, resizable );
	window = glfwCreateWindow( width, height, windowName, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL );
	if( window == NULL )
	{
		printf( "\n Failed to create GLFW window! " );
		return -1;
	}
	glfwGetFramebufferSize( window, &(this->width), &(this->height) );
	
	/*
	glfwSetKeyCallback( window, KeyCallback );
	glfwSetCursorPosCallback( window, MouseCallback );
	glfwSetScrollCallback( window, ScrollCallback );
	*/
	
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwMakeContextCurrent( window );
	glewExperimental = GL_TRUE;
	if ( GLEW_OK != glewInit( ) )
	{
	    printf( "\n Failed to initialize GLEW! " );
	    return EXIT_FAILURE;
	}
}

void OpenGL::InitGraphic()
{
	glViewport( 0, 0, width, height );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void OpenGL::InitFrame()
{
	glViewport( 0, 0, width, height );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}

void OpenGL::SwapBuffer()
{
	glFlush();
	glfwSwapBuffers( window );
}

void OpenGL::Destroy()
{
	glfwDestroyWindow( window );
	window = NULL;
	width = height = 0;
}

OpenGL::OpenGL()
{
	mouseLastX = mouseLastY = mouseCurrentX = mouseCurrentY = wheelDY = 0.0;
	keys.resize( 1024 );
	memset( keys, 0, keys.size()*sizeof(bool) );
	SwapInput();
}

OpenGL::~OpenGL()
{
	Destroy();
	glfwTerminate();
}

void KeyCallback( GLFWwindow * window, int key, int scancode, int action, int mode )
{
    if( key >= 0 && key <= 1024 )
    {
        if( GLFW_PRESS == action )
        {
            openGL.keys[key] = true;
        }
        else if( GLFW_RELEASE == action )
        {
            openGL.keys[key] = false;
        }
    }
}

void ScrollCallback( GLFWwindow * window, double xOffset, double yOffset )
{
	openGL.wheelDY += yOffset;
}

void MouseCallback( GLFWwindow * window, double xPos, double yPos )
{
	static bool firstMouse = true;
	if( firstMouse )
	{
		mouseLastX = mouseCurrentX;
		mouseLastY = mouseCurrentY;
		firstMouse = false;
	}
	mouseCurrentX = xPos;
	mouseCurrentY = yPos;
}

#endif

