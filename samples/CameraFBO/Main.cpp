
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
	std::shared_ptr<gl::Shader> shader2 = std::make_shared<gl::Shader>();
	shader2->Load(
			"../../samples/CameraFBO/vertex.glsl",
			NULL,
			"../../samples/CameraFBO/fragment.glsl");
	
	gl::BasicMeshLoader::StaticMeshRenderable
		terrain("../../samples/Terrain.fbx", 0, shader),
		monkey("../../samples/Monkey.fbx", 0, shader),
		cameraRenderMesh("../../samples/FlatTexturedSquare.fbx", 0, shader);
	
	// Get uniform locations
	int modelLoc = shader->GetUniformLocation("model");
	int viewLoc = shader->GetUniformLocation("view");
	int projLoc = shader->GetUniformLocation("projection");
	int lightDirLoc = shader->GetUniformLocation("lightDir");
	int useTexLoc = shader->GetUniformLocation("useTex");
	int texLoc = shader->GetUniformLocation("tex");
	
			printf("texLoc = %i\n", texLoc);
			printf("useTexLoc = %i\n", useTexLoc);
			printf("viewLoc = %i\n", viewLoc);
			printf("lightDirLoc = %i\n", lightDirLoc);
			printf("projLoc = %i\n", projLoc);
	
	Camera camera2(glm::vec3(0.0f,0.0f, 0.0f));
	
	auto RenderCommonObjects = [&](std::shared_ptr<gl::Shader> shader_){
		shader_->SetBool(useTexLoc, false);
		gl::openGL.FaceCulling(true, false);
		
		shader_->SetMat4(modelLoc,
				glm::rotate(
					glm::mat4(1),
					3.141592f/2.0f,
					glm::vec3(-1, 0, 0)
					));
		terrain.Draw();
		
		shader_->SetMat4(modelLoc,
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
	
	const uint32_t RENDER_SIZE = 256;
	
	// gen and setup FBO
	std::shared_ptr<gl::Texture> renderTargetTexture
		= std::make_shared<gl::Texture>();
	renderTargetTexture->UpdateTextureData(NULL, RENDER_SIZE, RENDER_SIZE,
			false, gl::TEXTURE_2D, gl::RGBA, gl::RGBA, gl::UNSIGNED_BYTE);
	renderTargetTexture->WrapX(gl::CLAMP_TO_BORDER);
	renderTargetTexture->WrapY(gl::CLAMP_TO_BORDER);
	renderTargetTexture->MinFilter(gl::NEAREST);
	renderTargetTexture->MagFilter(gl::MAG_NEAREST);
	
// 	renderTargetTexture->InitTextureEmpty(RENDER_SIZE, RENDER_SIZE,
// 			gl::TextureTarget::TEXTURE_2D,
// 			gl::TextureSizedInternalFormat::RGBA8UI);
	
// 	std::shared_ptr<gl::Texture> renderTargetDepth
// 		= std::make_shared<gl::Texture>();
// 	renderTargetDepth->UpdateTextureData(NULL, RENDER_SIZE, RENDER_SIZE,
// 			false, gl::TEXTURE_2D, gl::RGBA, gl::R32, gl::UNSIGNED_INT);
// 	renderTargetTexture->InitTextureEmpty(RENDER_SIZE, RENDER_SIZE,
// 			gl::TextureTarget::TEXTURE_2D,
// 			gl::TextureSizedInternalFormat::RGBA8UI);
	
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			renderTargetTexture->GetTexture(), 0);
// 	std::shared_ptr<gl::Texture> renderTargetDepth = std::make_shared<gl::Texture>();
// 	renderTargetDepth->UpdateTextureData(NULL, RENDER_SIZE, RENDER_SIZE, false, gl::TEXTURE_2D,
// 			(gl::TextureDataFormat)GL_DEPTH24_STENCIL8,
// 			(gl::TextureDataFormat)GL_DEPTH_STENCIL,
// 			(gl::DataType)GL_UNSIGNED_INT_24_8);
// 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, renderTargetDepth->GetTexture(), 0);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, RENDER_SIZE, RENDER_SIZE);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	} else {
		printf("No FB error\n");
	}
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
		shader2->SetVec4(lightDirLoc,
				glm::normalize(
					glm::rotate(
						glm::mat4(1),
						(lastFrame)/1.0f,
						glm::vec3(0,1,0)
					) * glm::vec4(-1,-0.3,-1,0)
				));
		
		// Render everything for secondary camera (FBO)
		{
			shader->Use();
			// Set up FBO
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
// 			shader->SetTexture(texLoc, NULL, 0);
			
			glViewport(0, 0, RENDER_SIZE, RENDER_SIZE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			// Calculate andset projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f));
			
			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera2.getViewMatrix());
			
			RenderCommonObjects(shader);
			
			// Set down FBO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			gl::openGL.InitFrame();
		}
		
		// Render primary camera
		{
			shader->Use();
			// Calculate and set projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f,
						(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
						10000.0f));

			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera.getViewMatrix());
			
			RenderCommonObjects(shader);
			
			// Render FBO result
			gl::openGL.FaceCulling(true, true);
			shader->SetBool(useTexLoc, true);
			shader->SetTexture(texLoc, renderTargetTexture.get(), 0);
			shader->SetMat4(modelLoc,
					glm::scale(
						glm::translate(
							glm::rotate(
								glm::rotate(
									glm::mat4(1),
									3.141592f/2.f,
									{0,1,0}
								),
								3.141592f/2.f,
								{0,0,1}
							),
							{0, 0, 5}
						),
						{5,-5,5}
					));
			cameraRenderMesh.Draw();
		}
		
		DefaultIterationEnd();
	}

	gl::openGL.Destroy();
	glfwTerminate();

	return EXIT_SUCCESS;
}
}


