
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../OpenGL.h"
#include "../../Shader.h"
#include "../../Texture.h"
#include "../../VAO.h"
#include "../../VBO.h"

bool keys[1024];

void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


#include <cstdio>

#pragma pack(push, 1)
struct Object {
	glm::vec2 pos;
	glm::vec2 uv;
	int counts;
	int padding1;
	int elems[32];
	int pad[26];
};
#pragma pack(pop, 1)

const uint32_t OBJECTS_COUNT = 1024*512*2;
const uint32_t COMPS = 32;

#include <map>
#include <unordered_map>
std::unordered_map<int, int> map1, map2;

#include <ctime>
int main() {
	srand(time(NULL));
	map1.reserve(1024*1024);
	map2.reserve(1024*1024);
    openGL.Init("Window test name 311", 800, 600, true, false);
    openGL.InitGraphic();
    openGL.SetKeyCallback(KeyCallback);
	
	Shader computeShader, emptyShader;
	computeShader.Load("compute.glsl");
	
	VBO indirectBuffer(32, gl::DRAW_INDIRECT_BUFFER, gl::STREAM_DRAW);
	VBO atomicBuffer(4, gl::SHADER_STORAGE_BUFFER, gl::DYNAMIC_DRAW);
	VBO infosBuffer(sizeof(Object), gl::ARRAY_BUFFER, gl::STREAM_DRAW);
	
	indirectBuffer.ReserveResizeVertices(OBJECTS_COUNT*32);
	indirectBuffer.Generate();
	atomicBuffer.ReserveResizeVertices(640);
	atomicBuffer.Generate();
	infosBuffer.ReserveResizeVertices(OBJECTS_COUNT);
    auto buf = infosBuffer.Buffer<Atr<Object, 1>>();
	int sum = 0;
	for(int i=0; i<OBJECTS_COUNT; ++i) {
		buf.At<0>(i).pos = {rand(), rand()};
		buf.At<0>(i).uv = {rand(), rand()};
		int coun = rand()%COMPS;

		buf.At<0>(i).counts = coun;
		for(int j=0; j<coun; ++j) {
			int v = (rand()%(1024*1024))+1;
			buf.At<0>(i).elems[j] = v;
			map1[v]++;
		}
		sum += buf.At<0>(i).counts;
	}
	printf("sum = %i\n\n", sum);
	infosBuffer.Generate();
	
	int FRAME = 0;
	
    while(!glfwWindowShouldClose(openGL.window) && FRAME<4) {
		++FRAME;
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        openGL.InitFrame();
        
		
		
		
		unsigned objectCountLoc = computeShader.GetUniformLocation("objectsCount");
		
		memset(&(atomicBuffer.Buffer()[0]), 0, atomicBuffer.Buffer().size());
		atomicBuffer.Update(0, 640);
		int innerIters = 1024;
		for(int i=0; i<innerIters; ++i) {
			memset(&(atomicBuffer.Buffer()[0]), 0, atomicBuffer.Buffer().size());
			atomicBuffer.Update(0, 16);
			computeShader.SetInt(objectCountLoc, OBJECTS_COUNT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, indirectBuffer.GetIdGL());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, infosBuffer.GetIdGL());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, atomicBuffer.GetIdGL());
			computeShader.Use();
			computeShader.Dispatch(
					32*(OBJECTS_COUNT-1+computeShader.workgroupSize[0])/
					computeShader.workgroupSize[0], 1, 1);
			emptyShader.Use();
			
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			atomicBuffer.FetchAllDataToHostFromGPU();
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}
		
		float T = (glfwGetTime()-currentFrame);
		
		indirectBuffer.FetchAllDataToHostFromGPU();
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		
// 		int components = *(unsigned*)&(atomicBuffer.Buffer()[0]);
		int components = sum;
		auto buf = indirectBuffer.Buffer<Atr<uint32_t, 5>>();
		for(int i=0; i<components; ++i) {
			if(buf.At<0>(i, 2)) {
				map2[buf.At<0>(i, 2)]++;
			}
		}
		
		std::vector<std::pair<int, int>> A, B;
		A.insert(A.begin(), map1.begin(), map1.end());
		B.insert(B.begin(), map2.begin(), map2.end());
		
		std::sort(A.begin(), A.end());
		std::sort(B.begin(), B.end());
		
		if(A == B) {
			printf("Maps are equal!\n");
		} else {
			printf("Maps are not equal!\n");
		}
		
		/*
		printf("\n\n");
		for(auto it : A) {
			printf(" %i->%i", it.first, it.second);
		}
		printf("\n\n");
		for(auto it : B) {
			printf(" %i->%i", it.first, it.second);
		}
		printf("\n\n");
		*/
		
		map2.clear();
		
		
		printf(" frame time = %f ms\n", T*1000.0);
		printf(" atomic[0] = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[0]));
		printf(" GlobalInvocationId.x = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[20]));
		printf(" GlobalInvocationId.y = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[24]));
		printf(" GlobalInvocationId.z = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[28]));
		printf(" LocalInvocation.x = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[32]));
		printf(" LocalInvocation.y = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[36]));
		printf(" LocalInvocation.z = %u\n", *(unsigned*)&(atomicBuffer.Buffer()[40]));
		
		
		uint64_t iterations = *(unsigned*)&(atomicBuffer.Buffer()[68]);
		iterations <<= 32;
		iterations += *(unsigned*)&(atomicBuffer.Buffer()[64]);
		
				iterations = (long long)components * (long long)innerIters;
		
		printf(" Iterations = %lu\n", iterations);
		printf(" iterations per second: %.2f Gops\n",
				((float)(iterations))/(1000.0*1000.0*1000.0*T));
		
		printf("\n");
    }
	openGL.Destroy();
	glfwTerminate();
    return EXIT_SUCCESS;
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

