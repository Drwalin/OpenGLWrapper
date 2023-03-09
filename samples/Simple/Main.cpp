
#include <cstdio>
#define DEBUG(x) 
//printf("\n %i",(int)x);

#include "../../include/openglwrapper/OpenGL.hpp"
#include "../../include/openglwrapper/Shader.hpp"
#include "../../include/openglwrapper/Texture.hpp"
#include "../../include/openglwrapper/VAO.hpp"
#include "../../include/openglwrapper/VBO.hpp"
#include "../../include/openglwrapper/BufferAccessor.hpp"

#include "../Camera.hpp"

#include "../DefaultCameraAndOtherConfig.hpp"

namespace Simple {

int main() {
	DefaultsSetup();

	gl::Shader ourShader;
	ourShader.Load(
			"../samples/Simple/vertex.glsl",
			"../samples/Simple/geometry.glsl",
			"../samples/Simple/fragment.glsl");


	gl::VBO vbo(3*sizeof(float), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	std::vector<uint8_t> Vbo;
	gl::BufferAccessor::BufferRef<gl::Atr<glm::vec3, 1>> buf(&vbo, Vbo);
	for(uint32_t i = 0; i < 8; ++i)
		buf.At<0>(i) = glm::vec3(i, i/2.f, i/3.f);
	vbo.Init();
	vbo.Generate(Vbo);

	gl::VAO vao(gl::POINTS);
	vao.Init();
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3,
			gl::FLOAT, false, 0);



	gl::Texture texture;
	texture.Load("../samples/Simple/image.jpg", false, 4);

	ourShader.SetTexture(ourShader.GetUniformLocation("ourTexture1"), &texture,
			0);

	glPointSize(3.0f);
	glLineWidth(3.0f);
	glEnable(GL_DITHER);


	GLint modelLoc = ourShader.GetUniformLocation("model");
	GLint viewLoc = ourShader.GetUniformLocation("view");
	GLint projLoc = ourShader.GetUniformLocation("projection");

	while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();

		ourShader.Use();

		glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);

		// Create transformations
		//         glm::mat4 model;
		glm::mat4 view;

		view = camera.getViewMatrix();

		ourShader.SetMat4(viewLoc, view);
		ourShader.SetMat4(projLoc, projection);

		for(int j = 1; j < 101; ++j) {
			for(GLuint i = 1; i < 11; ++i) {
				glm::mat4 model(1.0f);
				model = glm::scale(model, glm::vec3(10));
				model = glm::translate(model,
						glm::vec3(0.0f,0.0f,0.0f+float((j*i)<<1)));
				ourShader.SetMat4(modelLoc, model);
				vao.SetInstances(100*1);
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

