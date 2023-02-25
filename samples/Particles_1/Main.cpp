
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <functional>

#include "../Camera.hpp"

#define DEBUG(x) 
//printf("\n %i",(int)x);

#include "../DefaultCameraAndOtherConfig.hpp"
#include "openglwrapper/basic_mesh_loader/AssimpLoader.hpp"
#include "openglwrapper/basic_mesh_loader/Value.hpp"
#include "../../include/openglwrapper/BufferAccessor.hpp"

namespace Particles_1 {

float randf(float min, float max) {
	return (max-min)*(rand()) / (float)RAND_MAX + min;
}

int main() {
	DefaultsSetup();
	
	gl::Shader ourShader;
	ourShader.Load(
			"../samples/Particles_1/vertex.glsl",
			"../samples/Particles_1/geometry.glsl",
			"../samples/Particles_1/fragment.glsl");
	
	gl::VBO vbo(11*sizeof(float), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	std::vector<uint8_t> Vbo;
	gl::BufferAccessor::BufferRef<gl::Atr<glm::vec3, 2>, gl::Atr<float, 1>, gl::Atr<glm::vec4, 1>> buf(&vbo, Vbo);
	for(int i = 100000; i >= 0; --i) {
		auto rnd = std::bind(randf, -0.1, 0.1);
		buf.At<0>(i, 0) = glm::vec3(rnd(), rnd(), rnd())*0.4f;
		buf.At<0>(i, 1) = glm::vec3(rnd()*10, 20*rnd()+25, 3+rnd()*10);
		buf.At<1>(i) = randf(2.3f, 4.1f);
		float b = randf(0.7, 1);
		buf.At<2>(i) = glm::vec4(
				randf(0, 0.4) * b,
				randf(0, 0.6),
				b,
				randf(0, 1)
				);
	}
	vbo.Generate(Vbo);
	
	gl::VAO vao(gl::POINTS);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3,
			gl::FLOAT, false, 0);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("startVelocity"), 3,
			gl::FLOAT, false, 12);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("lifeTime"), 1,
			gl::FLOAT, false, 24);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4,
			gl::FLOAT, false, 28);
	
	unsigned timeUniformLocation = ourShader.GetUniformLocation("time");
	unsigned accelerationLoc = ourShader.GetUniformLocation("acceleration");
	
	gl::Texture texture;
	texture.Load("../samples/Simple/image.jpg", false);
	
	texture.MinFilter(gl::NEAREST);
	texture.MagFilter(gl::MAG_NEAREST);
	texture.WrapX(gl::REPEAT);
	texture.WrapY(gl::REPEAT);
	
	ourShader.SetTexture(ourShader.GetUniformLocation("ourTexture1"), &texture,
			0);
	
	while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
		
		ourShader.Use();
		
		glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);
		
		// Create transformations
		glm::mat4 view;

		view = camera.getViewMatrix();
		
		GLint modelLoc = ourShader.GetUniformLocation("model");
		GLint viewLoc = ourShader.GetUniformLocation("view");
		GLint projLoc = ourShader.GetUniformLocation("projection");
		
		ourShader.SetMat4(viewLoc, view);
		ourShader.SetMat4(projLoc, projection);
		ourShader.SetFloat(timeUniformLocation, lastFrame + 10.0f);
		ourShader.SetVec3(accelerationLoc, glm::vec3(0, -10, 0));
		
		for(int j = 0; j < 1; ++j) {
			for(GLuint i = 10; i < 11; ++i) {
				glm::mat4 model(1.0f);
				model = glm::scale(model, glm::vec3(10));
				model = glm::translate(model,
						glm::vec3(0.0f,0.0f,0.0f+float((j*i)<<1)));
				ourShader.SetMat4(modelLoc, model);
				//vao.SetInstances(1000*1000);
				vao.Draw();//0, 36);
			}
		}
		
		DefaultIterationEnd();
	}
	
	gl::openGL.Destroy();
	glfwTerminate();
	
	return EXIT_SUCCESS;
}
}

