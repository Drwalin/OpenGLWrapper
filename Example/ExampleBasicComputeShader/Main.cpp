
#include <cstdio>
#include <ctime>

#include <algorithm>

#include <openglwrapper/OpenGL.h>
#include <openglwrapper/Shader.h>
#include <openglwrapper/Texture.h>
#include <openglwrapper/VAO.h>
#include <openglwrapper/VBO.h>

const uint32_t OBJECTS_COUNT = 1024*32;

int main() {
	srand(time(NULL));
	
	// init open gl
    gl::openGL.Init("Window test name 311", 800, 600, true, false);
    gl::openGL.InitGraphic();
	
	// init shaders
	gl::Shader computeShader, emptyShader;
	computeShader.Load("../Example/ExampleBasicComputeShader/compute.glsl");
	
	// init data storage objects
	gl::VBO sourceBuffer(sizeof(uint32_t), gl::ARRAY_BUFFER, gl::DYNAMIC_DRAW);
	gl::VBO destinyBuffer(sizeof(uint32_t), gl::ARRAY_BUFFER, gl::DYNAMIC_DRAW);
	sourceBuffer.ReserveResizeVertices(OBJECTS_COUNT);
	destinyBuffer.ReserveResizeVertices(OBJECTS_COUNT);
	destinyBuffer.Generate();
	
	// init computational data values
    auto src = sourceBuffer.Buffer<gl::Atr<uint32_t, 1>>();
	uint32_t sum = 0;
	for(int i=0; i<OBJECTS_COUNT; ++i) {
		src.At<0>(i) = i;
		sum += src.At<0>(i);
	}
	// generate data and upload to gpu
	sourceBuffer.Generate();
	
	{
		// set shader uniform data
		computeShader.SetInt(computeShader.GetUniformLocation("objectsCount"), OBJECTS_COUNT);
		
		// set shader buffer data
		sourceBuffer.BindBufferBase(gl::SHADER_STORAGE_BUFFER, 4);
		destinyBuffer.BindBufferBase(gl::SHADER_STORAGE_BUFFER, 5);
		
		// call compute shader
		computeShader.Use();
		computeShader.DispatchRoundGroupNumbers(1024, 1, 1);
		emptyShader.Use();
				
		// fetch data
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		destinyBuffer.FetchAllDataToHostFromGPU();
		
		// validate data
		int correct=0, wrong=0;
		auto dst = destinyBuffer.Buffer<gl::Atr<uint32_t, 1>>();
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

