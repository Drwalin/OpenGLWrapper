
#include "../DefaultCameraAndOtherConfig.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/SimpleRender.hpp"
#include "openglwrapper/OpenGL.hpp"
#include "openglwrapper/Texture.hpp"

#include <cstring>
#include <filesystem>

namespace CameraFBO {
	
template<typename T>
void Copy(gl::VBO& vbo, std::vector<T>& v) {
	std::vector<uint8_t>& b = vbo.Buffer();
	b.resize(v.size()*sizeof(T));
	memcpy(&(b[0]), &(v[0]), b.size());
}
	
int main() {
	{
		auto path = std::filesystem::current_path();
		path.append("thirdparty");
		std::filesystem::current_path(path);
	}
	DefaultsSetup();

	// Load shader
	std::shared_ptr<gl::Shader> shader = std::make_shared<gl::Shader>();
	shader->Load(
			"../../samples/CameraFBO/vertex.glsl",
			NULL,
			"../../samples/CameraFBO/fragment.glsl");
	
	gl::BasicMeshLoader::StaticMeshRenderable
		terrain("../../samples/Terrain.fbx", 0, shader),
		monkey("../../samples/Monkey.fbx", 0, shader),
		cameraRenderMesh("../../samples/FlatTexturedSquare.fbx", 0, shader);
	
	std::shared_ptr<gl::Texture> renderTargetTexture = std::make_shared<gl::Texture>();
	renderTargetTexture->InitTextureEmpty(1024, 1024, gl::TextureTarget::TEXTURE_2D, gl::TextureSizedInternalFormat::RGBA8UI);
	
	
	// Get uniform locations
	int modelLoc = shader->GetUniformLocation("model");
	int viewLoc = shader->GetUniformLocation("view");
	int projLoc = shader->GetUniformLocation("projection");
	int lightDirLoc = shader->GetUniformLocation("lightDir");
	int useTexLoc = shader->GetUniformLocation("useTex");
	int texLoc = shader->GetUniformLocation("tex");
	
	Camera camera2;
	
	auto RenderCommonObjects = [&](){
		shader->SetBool(useTexLoc, false);
		gl::openGL.FaceCulling(true, false);
		
		shader->SetMat4(modelLoc,
				glm::rotate(
					glm::mat4(1),
					3.141592f/2.0f,
					glm::vec3(-1, 0, 0)
					));
		terrain.Draw();
		
		shader->SetMat4(modelLoc,
				glm::rotate(
					glm::rotate(
						glm::translate(
							glm::mat4(1),
							glm::vec3(5, 0, 0)
							),
						(lastFrame*0.7f)/1.0f,
						glm::vec3(0,1,0)
						),
					3.141592f/2.0f,
					glm::vec3(-1, 0, 0)
					));
		monkey.Draw();
	};
	
	// gen and setup FBO
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			renderTargetTexture->GetTexture(), 0);
// 	std::shared_ptr<gl::Texture> renderTargetDepth = std::make_shared<gl::Texture>();
// 	renderTargetDepth->UpdateTextureData(NULL, 1024, 1024, false, gl::TEXTURE_2D,
// 			(gl::TextureDataFormat)GL_DEPTH24_STENCIL8,
// 			(gl::TextureDataFormat)GL_DEPTH_STENCIL,
// 			(gl::DataType)GL_UNSIGNED_INT_24_8);
// 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, renderTargetDepth->GetTexture(), 0);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
		
		// Use shader
		shader->Use();
		
		// Set light direction
		shader->SetVec4(lightDirLoc,
				glm::normalize(
					glm::rotate(
						glm::mat4(1),
						(lastFrame)/1.0f,
						glm::vec3(0,1,0)
					) * glm::vec4(-1,-0.3,-1,0)
				));
		
		// Render everything for secondary camera (FBO)
		{
			// Set up FBO
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
			glEnable(GL_DEPTH_TEST);
			shader->SetTexture(texLoc, NULL, 0);
			
			// Calculate andset projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f));

			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera2.getViewMatrix());
			
			RenderCommonObjects();
			
			// Set down FBO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		
		// Render primary camera
		{
			// Calculate andset projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f,
						(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
						10000.0f));

			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera.getViewMatrix());
			
			RenderCommonObjects();
			
			// Render FBO result
			gl::openGL.FaceCulling(true, true);
			shader->SetBool(useTexLoc, true);
			shader->SetTexture(texLoc, renderTargetTexture.get(), 0);
			shader->SetMat4(modelLoc, glm::mat4(1));
			cameraRenderMesh.Draw();
		}
		
		DefaultIterationEnd();
	}

	gl::openGL.Destroy();
	glfwTerminate();

	return EXIT_SUCCESS;
}
}


