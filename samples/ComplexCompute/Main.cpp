
#include <algorithm>
#include <cstdio>
#include <map>
#include <unordered_map>
#include <ctime>

#include "../../include/openglwrapper/OpenGL.hpp"
#include "../../include/openglwrapper/Shader.hpp"
#include "../../include/openglwrapper/Texture.hpp"
#include "../../include/openglwrapper/VAO.hpp"
#include "../../include/openglwrapper/VBO.hpp"
#include "../../include/openglwrapper/BufferAccessor.hpp"

namespace ComplexCompute {

bool keys[1024];

void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

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

std::unordered_map<int, int> map1, map2;

int main() {
	srand(time(nullptr));
	map1.reserve(1024*1024);
	map2.reserve(1024*1024);
	gl::openGL.Init("Window test name 311", 800, 600, true, false);
    gl::openGL.InitGraphic();
    gl::openGL.SetKeyCallback(KeyCallback);
	
	gl::Shader computeShader, emptyShader;
	computeShader.Load("../samples/ComplexCompute/compute.glsl");
	
	gl::VBO indirectBuffer(32, gl::DRAW_INDIRECT_BUFFER, gl::STREAM_DRAW);
	indirectBuffer.Init();
	gl::VBO atomicBuffer(4, gl::SHADER_STORAGE_BUFFER, gl::DYNAMIC_DRAW);
	atomicBuffer.Init();
	gl::VBO infosBuffer(sizeof(Object), gl::ARRAY_BUFFER, gl::STREAM_DRAW);
	infosBuffer.Init();
	
	std::vector<uint8_t> indirectVbo, atomicVbo, infosVbo;
	
	indirectBuffer.Generate(nullptr, OBJECTS_COUNT*32);
	indirectVbo.resize(OBJECTS_COUNT*32*indirectBuffer.VertexSize());
	atomicBuffer.Generate(nullptr, 640);
	atomicVbo.resize(640*atomicBuffer.VertexSize());
	infosVbo.resize(OBJECTS_COUNT*infosBuffer.VertexSize());
	gl::BufferAccessor::BufferRef<gl::Atr<Object, 1>> buf(&infosBuffer, infosVbo);
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
	infosBuffer.Generate(infosVbo);
	
	int FRAME = 0;
	
    while(!glfwWindowShouldClose(gl::openGL.window) && FRAME<4) {
		++FRAME;
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        gl::openGL.InitFrame();
        
		
		
		
		unsigned objectCountLoc = computeShader.GetUniformLocation("objectsCount");
		
		memset(&(atomicVbo[0]), 0, atomicVbo.size());
		atomicBuffer.Generate(atomicVbo);
		int innerIters = 1024;
		for(int i=0; i<innerIters; ++i) {
			memset(&(atomicVbo[0]), 0, atomicVbo.size());
			atomicBuffer.Generate(atomicVbo);
			computeShader.SetInt(objectCountLoc, OBJECTS_COUNT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, indirectBuffer.GetIdGL());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, infosBuffer.GetIdGL());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, atomicBuffer.GetIdGL());
			computeShader.Use();
			computeShader.Dispatch(
					32*(OBJECTS_COUNT-1+computeShader.workgroupSize[0])/
					computeShader.workgroupSize[0], 1, 1);
			emptyShader.Use();
			
			gl::MemoryBarrier(gl::ALL_BARRIER_BITS);
			atomicBuffer.FetchAll(atomicVbo);
			
			gl::MemoryBarrier(gl::ALL_BARRIER_BITS);
		}
		
		float T = (glfwGetTime()-currentFrame);
		
		indirectBuffer.FetchAll(indirectVbo);
		infosBuffer.FetchAll(infosVbo);
		atomicBuffer.FetchAll(atomicVbo);
		gl::MemoryBarrier(gl::ALL_BARRIER_BITS);
		
// 		int components = *(unsigned*)&(atomicBuffer.Buffer()[0]);
		int components = sum;
// 		auto buf = indirectBuffer.Buffer<gl::Atr<uint32_t, 5>>();
		gl::BufferAccessor::BufferRef<gl::Atr<uint32_t, 5>> buf(&infosBuffer, infosVbo);
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
		printf(" atomic[0] = %u\n", *(unsigned*)&(atomicVbo[0]));
		printf(" GlobalInvocationId.x = %u\n", *(unsigned*)&(atomicVbo[20]));
		printf(" GlobalInvocationId.y = %u\n", *(unsigned*)&(atomicVbo[24]));
		printf(" GlobalInvocationId.z = %u\n", *(unsigned*)&(atomicVbo[28]));
		printf(" LocalInvocation.x = %u\n", *(unsigned*)&(atomicVbo[32]));
		printf(" LocalInvocation.y = %u\n", *(unsigned*)&(atomicVbo[36]));
		printf(" LocalInvocation.z = %u\n", *(unsigned*)&(atomicVbo[40]));
		
		
		uint64_t iterations = *(unsigned*)&(atomicVbo[68]);
		iterations <<= 32;
		iterations += *(unsigned*)&(atomicVbo[64]);
		
				iterations = (long long)components * (long long)innerIters;
		
		printf(" Iterations = %lu\n", iterations);
		printf(" iterations per second: %.2f Gops\n",
				((float)(iterations))/(1000.0*1000.0*1000.0*T));
		
		printf("\n");
    }
	gl::openGL.Destroy();
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

}

