
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Camera.cpp"

#include <cstdio>

GLenum errorCheck(int line, const char* file) {
	GLenum code;
	const GLubyte* string;
	code = glGetError();
	if(code != GL_NO_ERROR) {
		string = gluErrorString(code);
		fprintf(stderr, "%s:%i -> OpenGL error [%i]: %s\n", file, line, code, string);
		exit(311);
	}
	return code;
}

#define PRINT_ERROR errorCheck(__LINE__, __FILE__);


// Window dimensions
//const GLuint WIDTH = 800, HEIGHT = 600;
//int SCREEN_WIDTH, SCREEN_HEIGHT;
void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow * window, double xOffset, double yOffset);
void MouseCallback(GLFWwindow * window, double xPos, double yPos);
void DoMovement();

#include "../../OpenGL.h"
#include "../../Shader.h"
#include "../../Texture.h"
#include "../../VAO.h"
#include "../../VBO.h"

int32_t LoadComponents(VBO& vertices, VBO& elements);

Camera camera(glm::vec3(0.0f,0.0f, 0.0f));
GLfloat lastX = 0.0;//WIDTH/2.0;
GLfloat lastY = 0.0;//WIDTH/2.0;
bool keys[1024];
bool firstMouse = true;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

#include <cstdio>
#include <ctime>
#include <cstring>


#pragma pack(push, 1)
struct VertexStructure {
	glm::vec3 pos;
	glm::vec2 uv[2];
	uint8_t color[4];
};
#pragma pack(pop, 1)

struct PerInstance {
	glm::mat4 model;
	unsigned objectId;
	unsigned componentId;
	unsigned textureId;
	uint8_t color[4];
};

const uint32_t OBJECTS_COUNT = 1024;


VertexStructure VERTEEE;

int main() {
	camera.position = {50, 50, 50};

	PRINT_ERROR;
	srand(time(NULL));
	printf("sizeof(VertexStructure) = %lu\n", sizeof(VertexStructure));
	printf("sizeof(PerInstance) = %lu\n", sizeof(PerInstance));

	openGL.Init("Window test name 311", 800, 600, true, false, 4, 5);
	openGL.InitGraphic();

	PRINT_ERROR;
	openGL.SetKeyCallback(KeyCallback);
	openGL.SetScrollCallback(ScrollCallback);
	openGL.SetMouseCallback(MouseCallback);

	PRINT_ERROR;
	Shader renderShader;
	renderShader.Load("vertex.glsl", NULL, "fragment.glsl");
	PRINT_ERROR;

	VBO vertexBuffer(32, gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	VBO elementsBuffer(sizeof(uint32_t), gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	VBO indirectBuffer(20, gl::DRAW_INDIRECT_BUFFER, gl::STATIC_DRAW);
	VBO perInstanceBuffer(sizeof(PerInstance), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	
	LoadComponents(vertexBuffer, elementsBuffer);

	PRINT_ERROR;
	vertexBuffer.Generate();
	PRINT_ERROR;
	vertexBuffer.Update(0, 1000000000);
	PRINT_ERROR;
	elementsBuffer.Generate();
	PRINT_ERROR;
	elementsBuffer.Update(0, 1000000000);
	PRINT_ERROR;



	auto indirect = indirectBuffer.Buffer<Atr<int, 5>>();
	auto perInstance = perInstanceBuffer.Buffer<Atr<PerInstance, 1>>();
	int I = 0;
	for(int i=0; i<OBJECTS_COUNT; ++i) {
		int components = (rand()%30) + 3;
		for(int j=0; j<components; ++j, ++I) {
			indirect.At<0>(I, 0) = 9;
			indirect.At<0>(I, 1) = 1;
			indirect.At<0>(I, 2) = (rand()%12)*3;
			indirect.At<0>(I, 3) = 0;
			indirect.At<0>(I, 4) = I;

			perInstance.At<0>(I).model = glm::mat4(
					0.3, 0  , 0  , rand() % 1000,
					0  , 0.3, 0  , rand() % 1000,
					0  , 0  , 0.3, rand() % 1000,
					0  , 0  , 0  , 1);
			perInstance.At<0>(I).componentId = j;
			perInstance.At<0>(I).objectId = i;
			perInstance.At<0>(I).textureId = rand()%256;
			uint8_t arr[4];
			arr[0] = rand()%256;
			arr[1] = rand()%256;
			arr[2] = rand()%256;
			arr[0] = 255;
			memcpy(perInstance.At<0>(I).color, arr, 4);
		}
	}
	PRINT_ERROR;
	indirectBuffer.Generate();
	PRINT_ERROR;
	indirectBuffer.Update(0,	1000000000);
	PRINT_ERROR;
	perInstanceBuffer.Generate();
	PRINT_ERROR;
	perInstanceBuffer.Update(0, 1000000000);
	PRINT_ERROR;

	VAO drawVao(gl::TRIANGLES);
	PRINT_ERROR;

	drawVao.SetAttribPointer(vertexBuffer,
			renderShader.GetAttributeLocation("pos"),
			3, gl::FLOAT, false, 0, 0);
	PRINT_ERROR;
	drawVao.SetAttribPointer(vertexBuffer,
			1,//renderShader.GetAttributeLocation("uv"),
			2, gl::SHORT, false, 12, 0);
	PRINT_ERROR;
	drawVao.SetAttribPointer(vertexBuffer,
			renderShader.GetAttributeLocation("color"),
			4, gl::UNSIGNED_BYTE, false, 20, 0);
	PRINT_ERROR;

	drawVao.SetAttribPointer(perInstanceBuffer,
			renderShader.GetAttributeLocation("model"),
			4, gl::FLOAT, false, 0, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			renderShader.GetAttributeLocation("model")+1,
			4, gl::FLOAT, false, 16, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			renderShader.GetAttributeLocation("model")+2,
			4, gl::FLOAT, false, 32, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			renderShader.GetAttributeLocation("model")+3,
			4, gl::FLOAT, false, 48, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			7,//renderShader.GetAttributeLocation("objectId"),
			1, gl::UNSIGNED_INT, false, 64, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			8,//renderShader.GetAttributeLocation("componentId"),
			1, gl::UNSIGNED_INT, false, 68, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			9,//renderShader.GetAttributeLocation("textureId"),
			1, gl::UNSIGNED_INT, false, 72, 1);
	drawVao.SetAttribPointer(perInstanceBuffer,
			10,//renderShader.GetAttributeLocation("objectColor"),
			4, gl::UNSIGNED_BYTE, false, 76, 1);
	PRINT_ERROR;

	drawVao.SetAttribPointer(elementsBuffer, -1, -1, gl::UNSIGNED_INT, false, 0, 0);
	PRINT_ERROR;

// 	drawVao.SetAttribPointer(indirectBuffer, -1, -1, gl::UNSIGNED_INT, false, 0, 0);
// 	PRINT_ERROR;
	
// 	drawVao.SetInstances(I);
// 	drawVao.SetSize(36);

// 	Texture texture;
// 	texture.Load("image.jpg", GL_REPEAT, GL_NEAREST, false);
	PRINT_ERROR;
// 	renderShader.SetTexture(renderShader.GetUniformLocation("ourTexture1"), &texture, 0);
	PRINT_ERROR;
	GLint viewProjectionLoc = renderShader.GetUniformLocation("viewProjection");
	PRINT_ERROR;

	printf(" I = %i\n", I);
	while(!glfwWindowShouldClose(openGL.window)) {
	PRINT_ERROR;
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

	PRINT_ERROR;
		glfwPollEvents();
	PRINT_ERROR;
		DoMovement();
	PRINT_ERROR;

		openGL.InitFrame();

	PRINT_ERROR;
		renderShader.Use();
	PRINT_ERROR;

		glm::mat4 projection = glm::perspective(45.0f,
				(float)openGL.GetWidth()/(float)openGL.GetHeight(), 0.1f,
				10000.0f);
		glm::mat4 view = camera.getViewMatrix();

	PRINT_ERROR;
		renderShader.SetMat4(viewProjectionLoc, view*projection);
	PRINT_ERROR;
	
// 		printf(" vao: sizeA = %i, sizeI = %i, instances = %i, drawArrays = %i\n",
// 				drawVao.sizeA, drawVao.sizeI, drawVao.instances, drawVao.drawArrays?1:0);

//		drawVao.DrawMultiElementsIndirect(&(indirectBuffer.Buffer()[0]), I, 0);
// 		drawVao.DrawMultiElementsIndirect(NULL, 1, 0);
	PRINT_ERROR;
		drawVao.Draw();
	PRINT_ERROR;

		openGL.SwapBuffer();
	PRINT_ERROR;
	}
	PRINT_ERROR;

	openGL.Destroy();
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

int32_t LoadComponents(VBO& vertices, VBO& elements) {
	auto buf = vertices.Buffer<Atr<VertexStructure, 1>>();
	vertices.ReserveResizeVertices(8);
	elements.ReserveResizeVertices(12*3);
	buf.At<0>(0).pos = glm::vec3(-1, -1, 1);
	buf.At<0>(1).pos = glm::vec3(-1, 1, 1);
	buf.At<0>(2).pos = glm::vec3(-1, -1, -1);
	buf.At<0>(3).pos = glm::vec3(-1, 1, -1);
	buf.At<0>(4).pos = glm::vec3(1, -1, 1);
	buf.At<0>(5).pos = glm::vec3(1, 1, 1);
	buf.At<0>(6).pos = glm::vec3(1, -1, -1);
	buf.At<0>(7).pos = glm::vec3(1, 1, -1);

	for(int i=0; i<8; ++i) {
		uint8_t arr[4];
		arr[0] = rand()%256;
		arr[1] = rand()%256;
		arr[2] = rand()%256;
		arr[3] = 255;
		memcpy(((uint8_t*)&(buf.At<0>(i)))+20, arr, 4);
	}

	uint32_t elems[] = {
		2, 3, 1,
		4, 7, 3,
		8, 5, 7,
		6, 1, 5,
		7, 1, 3,
		4, 6, 8,
		2, 4, 3,
		4, 8, 7,
		8, 6, 5,
		6, 2, 1,
		7, 5, 1,
		4, 2, 6
	};
	for(int i=0; i<36; ++i)
		elems[i]--;

	memcpy(&(vertices.Buffer()[0]), elems, 4*3*12);

	return 1;
}


