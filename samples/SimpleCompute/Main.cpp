
#include <cstdio>
#include <ctime>

#include <algorithm>

#include "../../include/openglwrapper/OpenGL.hpp"
#include "../../include/openglwrapper/Shader.hpp"
#include "../../include/openglwrapper/Texture.hpp"
#include "../../include/openglwrapper/VAO.hpp"
#include "../../include/openglwrapper/VBO.hpp"
#include "../../include/openglwrapper/BufferAccessor.hpp"

namespace SimpleCompute {
	
const uint32_t OBJECTS_COUNT = 1024*32;

int main() {
	srand(time(nullptr));
	
	// init open gl
    gl::openGL.Init("Window test name 311", 800, 600, true, false);
    gl::openGL.InitGraphic();
	
	// init shaders
	gl::Shader computeShader, emptyShader;
	computeShader.Load("../samples/SimpleCompute/compute.glsl");
	
	// init data storage objects
	gl::VBO sourceBuffer(sizeof(uint32_t), gl::ARRAY_BUFFER, gl::DYNAMIC_DRAW);
	sourceBuffer.Init();
	gl::VBO destinyBuffer(sizeof(uint32_t), gl::ARRAY_BUFFER, gl::DYNAMIC_DRAW);
	destinyBuffer.Init();
	std::vector<uint8_t> Src, Dst;
	Src.resize(OBJECTS_COUNT*4);
	Dst.resize(OBJECTS_COUNT*4);
	destinyBuffer.Generate(Dst);
	
	// init computational data values
	gl::BufferAccessor::BufferRef<gl::Atr<uint32_t, 1>> src(&sourceBuffer, Src);
	uint32_t sum = 0;
	for(int i=0; i<OBJECTS_COUNT; ++i) {
		src.At<0>(i) = i;
		sum += src.At<0>(i);
	}
	// generate data and upload to gpu
	sourceBuffer.Generate(Src);
	
	{
		// set shader uniform data
		computeShader.SetInt(computeShader.GetUniformLocation("objectsCount"), OBJECTS_COUNT);
		
		// set shader buffer data
		sourceBuffer.BindBufferBase(gl::SHADER_STORAGE_BUFFER, 4);
		destinyBuffer.BindBufferBase(gl::SHADER_STORAGE_BUFFER, 5);
		
		// call compute shader
		computeShader.Use();
		computeShader.Dispatch(128, 1, 1);
		emptyShader.Use();
				
		// fetch data
		gl::MemoryBarrier(gl::ALL_BARRIER_BITS);
		destinyBuffer.FetchAll(Dst);
		
		// validate data
		int correct=0, wrong=0;
		gl::BufferAccessor::BufferRef<gl::Atr<uint32_t, 1>> dst(&destinyBuffer, Dst);
		for(int i=0; i<OBJECTS_COUNT; ++i) {
			uint32_t a = src.At<0>(i);
			uint32_t b = dst.At<0>(i);
			if(a*sum == b)
				correct++;
			else
				wrong++;
		}
		printf(" Correct = %i\n   Wrong = %i\n", correct, wrong);
	}
		
	// deinit opengl
	gl::openGL.Destroy();
	glfwTerminate();
    return 0;
}

}

