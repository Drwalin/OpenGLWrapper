
#ifndef OPEN_GL_ENGINE_H
#define OPEN_GL_ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "C:\programowanie\cpp\OpenGL\SpartanJ-soil2-54073b423037\src\SOIL2\SOIL2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "lib/Array.cpp"
#include "lib/File.cpp"
#include "lib/String.cpp"

void OpenGLKeyCallback( GLFWwindow * window, int key, int scancode, int action, int mode );
void OpenGLScrollCallback( GLFWwindow * window, double xOffset, double yOffset );
void OpenGLMouseCallback( GLFWwindow * window, double xPos, double yPos );

class OpenGL
{
private:
public:
	
	Array < bool > keys;
	Array < bool > bKeys;
	double mouseLastX, mouseLastY;
	double mouseCurrentX, mouseCurrentY;
	double wheelDY;
	unsigned int width, height;
	GLFWwindow * window;
	
public:
	
	void SwapInput();
	bool IsKeyDown( const int id ) const;
	bool IsKeyUp( const int id ) const;
	bool WasKeyPressed( const int id ) const;
	bool WasKeyReleased( const int id ) const;
	double GetMouseDX() const;
	double GetMouseDY() const;
	double GetWheelDY() const;
	
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	
	int Init( const char * windowName, unsigned int width, unsigned int height, bool resizable, bool fullscreen );
	
	void InitGraphic();
	void InitFrame();
	void SwapBuffer();
	
	void Destroy();
	
	OpenGL();
	~OpenGL();
} openGL;

#endif

