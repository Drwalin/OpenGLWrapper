

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

#include "../../OpenGL.h"
#include "../../Shader.h"
#include "../../Texture.h"
#include "../../VAO.h"
#include "../../VBO.h"


Camera camera(glm::vec3(0.0f,0.0f, 0.0f));
GLfloat lastX = 0.0;//WIDTH/2.0;
GLfloat lastY = 0.0;//WIDTH/2.0;
bool keys[1024];
bool firstMouse = true;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


#include <cstdio>

#pragma pack(push, 1)
struct VertexStructure {
	glm::vec3 pos, normal, tan;
	glm::vec2 uv;
	glm::vec4 colors;
	
	/*
	float pX, pY, pZ;
	int16_t nX, nY, nZ;
	int16_t tX, tY, tZ;
	int16_t U, V;
	int8_t R, G, B, A;
	uint8_t bones[4];
	uint8_t weights[4];
	*/
};

const uint32_t MAX_COMPONENTS_PER_OBJECT = 32;

struct ObjectInfo {
	glm::mat4 model;
	glm::mat4 bones[64];
	uint32_t texture[MAX_COMPONENTS_PER_OBJECT];
	uint32_t componentsCount;
};

struct ObjectComponents {
	uint32_t elementOffset[MAX_COMPONENTS_PER_OBJECT];
	uint32_t indexCount[MAX_COMPONENTS_PER_OBJECT];
};
#pragma pack(pop, 1)

const uint32_t OBJECTS_COUNT = 1024*16;

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
		buf.At<0>(i).colors = {rand()/65000.0,rand()/65000.0,rand()/65000.0,0.5};
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
	
	memcpy(&(vertices.Buffer()[0]), elems, 4*3*12);
	
	return 1;
}

int main() {
	printf("sizeof(VertexStructure) = %lu\n", sizeof(VertexStructure));
	printf("&VertexStructure::colors = %lu\n", &VertexStructure::colors);
	
    openGL.Init("Window test name 311", 800, 600, true, false);
    openGL.InitGraphic();
    
    openGL.SetKeyCallback(KeyCallback);
    openGL.SetScrollCallback(ScrollCallback);
    openGL.SetMouseCallback(MouseCallback);
    
    Shader renderShader;
	renderShader.Load("vertex.glsl", "geometry.glsl", "fragment.glsl");
	
	Shader computeShader;
	computeShader.Load("compute.glsl");
	
	VBO vertexBuffer(sizeof(VertexStructure), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	VBO elementsBuffer(sizeof(uint32_t), gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	VBO transformsBuffer(sizeof(ObjectInfo), gl::ARRAY_BUFFER, gl::STREAM_DRAW);
	VBO componentsBuffer(sizeof(ObjectComponents), gl::ARRAY_BUFFER, gl::STREAM_DRAW);
	VBO indirectBuffer(20, gl::DRAW_INDIRECT_BUFFER, gl::STREAM_DRAW);
	VBO atomicBuffer(4, gl::ATOMIC_COUNTER_BUFFER, gl::DYNAMIC_DRAW);
	
	int32_t loadedComponents = LoadComponents(vertexBuffer, elementsBuffer);
	transformsBuffer.ReserveResizeVertices(OBJECTS_COUNT);
	transformsBuffer.Generate();
	componentsBuffer.ReserveResizeVertices(OBJECTS_COUNT);
	componentsBuffer.Generate();
	indirectBuffer.ReserveResizeVertices(OBJECTS_COUNT*MAX_COMPONENTS_PER_OBJECT);
	indirectBuffer.Generate();
	atomicBuffer.ReserveResizeVertices(1);
	atomicBuffer.Generate();
	
	VAO drawVao(gl::TRIANGLES);
	drawVao.SetAttribPointer(vertexBuffer,
			renderShader.GetAttributeLocation("pos"),
			3, gl::FLOAT, false, 0, 0);
	VertexStructure v;
	drawVao.SetAttribPointer(vertexBuffer,
			renderShader.GetAttributeLocation("color"),
			4, gl::FLOAT, false, (size_t)&v.colors - (size_t)&v, 0);
	drawVao.SetAttribPointer(transformsBuffer,
			renderShader.GetAttributeLocation("model"),
			16, gl::FLOAT, false, 0, 1);
	
    auto components = componentsBuffer.Buffer<Atr<ObjectComponents, 1>>();
    auto obs = transformsBuffer.Buffer<Atr<ObjectInfo, 1>>();
	for(int i=0; i<OBJECTS_COUNT; ++i) {
		obs.At<0>(i).componentsCount = (rand()%20)+3;
		obs.At<0>(i).model = glm::mat4(
				0.3, 0  , 0  , rand(),
				0  , 0.3, 0  , rand() % 100,
				0  , 0  , 0.3, rand(),
				0  , 0  , 0  , 1);
		for(int j=0; j<obs.At<0>(i).componentsCount; ++j) {
			components.At<0>(i).elementOffset[j] = 0;
			components.At<0>(i).indexCount[j] = 36;
		}
	}
	componentsBuffer.Update(0, OBJECTS_COUNT);
	transformsBuffer.Update(0, OBJECTS_COUNT);
	
	
    
    
	
    VBO vbo(3*sizeof(float), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
    auto buf = vbo.Buffer<Atr<glm::vec3, 1>>();
    for(int i = 0; i < 8; ++i)
        buf.At<0>(i) = glm::vec3(i, i/2.f, i/3.f);
    vbo.Generate();
    
    VAO vao(gl::POINTS);
	vao.SetAttribPointer(vbo, renderShader.GetAttributeLocation("position"), 3,
			gl::FLOAT, false, 0);
    
    
    
	Texture texture;
    texture.Load("image.jpg", GL_REPEAT, GL_NEAREST, false);
    
    renderShader.SetTexture(renderShader.GetUniformLocation("ourTexture1"), &texture,
			0);
    
	glPointSize(3.0f);
	glLineWidth(3.0f);
	glEnable(GL_DITHER);
	
	
	GLint viewLoc = renderShader.GetUniformLocation("view");
	GLint projLoc = renderShader.GetUniformLocation("projection");
	
    while(!glfwWindowShouldClose(openGL.window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents();
        DoMovement();
        
        openGL.InitFrame();
        
		
		
		
		memset(&(atomicBuffer.Buffer()[0]), 0, 4);
		atomicBuffer.Update(0, 1);
		computeShader.Dispatch(1024, 16, 1);
		glMemoryBarrier(-1);
		atomicBuffer.FetchAllDataToHostFromGPU();
		printf(" atomic = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[0]));
		
		
		
        
        
        
        renderShader.Use();
        
        glm::mat4 projection = glm::perspective(45.0f,
				(float)openGL.GetWidth()/(float)openGL.GetHeight(), 0.1f,
				10000.0f);
        
        // Create transformations
//         glm::mat4 model;
        glm::mat4 view;

        view = camera.getViewMatrix();
        
        renderShader.SetMat4(viewLoc, view);
        renderShader.SetMat4(projLoc, projection);
        
		/*
        for(int j = 1; j < 101; ++j) {
	        for(GLuint i = 1; i < 11; ++i) {
	            glm::mat4 model(1.0f);
                model = glm::scale(model, glm::vec3(10));
                model = glm::translate(model,
						glm::vec3(0.0f,0.0f,0.0f+float((j*i)<<1)));
                renderShader.SetMat4(modelLoc, model);
                vao.SetInstances(100*1);
                vao.Draw();//0, 36);
	        }
	    }
		*/
        
        openGL.SwapBuffer();
    }
	
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



