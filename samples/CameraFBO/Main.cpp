
#include "../DefaultCameraAndOtherConfig.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/SimpleRender.hpp"
#include "../../include/openglwrapper/OpenGL.hpp"
#include "../../include/openglwrapper/Texture.hpp"
#include "../../include/openglwrapper/FBO.hpp"

#include <cstring>
#include <filesystem>

namespace CameraFBO {
	
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
			"",
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
	
	Camera camera2(glm::vec3(0.0f,0.0f, 0.0f));
	
	auto RenderCommonObjects = [&](std::shared_ptr<gl::Shader> shader_){
		shader_->SetBool(useTexLoc, false);
		gl::openGL.FaceCulling(true, false);
		
		shader_->SetMat4(modelLoc, glm::mat4(1));
		terrain.Draw();
		
		shader_->SetMat4(modelLoc,
					glm::rotate(
						glm::translate(
							glm::mat4(1),
							glm::vec3(5, 0, 0)
							),
						(lastFrame*0.7f)/1.0f,
						glm::vec3(0,1,0)
					));
		monkey.Draw();
	};
	
	const uint32_t RENDER_SIZE = 256;
	
	// gen and setup FBO
	gl::Texture renderTargetTexture;
	renderTargetTexture.UpdateTextureData(nullptr, RENDER_SIZE, RENDER_SIZE,
			false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)gl::RGBA, gl::RGBA, gl::UNSIGNED_BYTE);
	renderTargetTexture.SetDefaultParamPixelartClampBorderNoMipmap();
	
	gl::Texture renderTargetDepth;
	renderTargetDepth.UpdateTextureData(nullptr, RENDER_SIZE, RENDER_SIZE, false,
			gl::TEXTURE_2D,
			(gl::TextureSizedInternalFormat)GL_DEPTH24_STENCIL8,
			(gl::TextureDataFormat)GL_DEPTH_STENCIL,
			(gl::DataType)GL_UNSIGNED_INT_24_8);
	
	gl::FBO fbo;
	fbo.AttachTexture(&renderTargetTexture, gl::ATTACHMENT_COLOR0, 0);
	fbo.AttachTexture(&renderTargetDepth, gl::ATTACHMENT_DEPTH_STENCIL, 0);
	
	GLenum Status = fbo.CheckStatus();
	if(Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	
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
			shader->Use();
			// Set up FBO
			fbo.Bind();
			shader->SetTexture(texLoc, nullptr, 0);
			
			// Set Viewport
			fbo.SetClearColor({0.2,0.3,8,0.5});
			fbo.Clear(true, true);
			fbo.Viewport(0, 0, RENDER_SIZE, RENDER_SIZE);
			
			// Calculate andset projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f));
			
			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera2.getViewMatrix());
			
			RenderCommonObjects(shader);
			
			// Set down FBO
			fbo.Unbind();
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
			shader->SetTexture(texLoc, &renderTargetTexture, 0);
			shader->SetMat4(modelLoc,
					glm::scale(
						glm::rotate(
							glm::translate(
								glm::mat4(1),
								{6, 0, 0}
							),
							3.141592f/2.f,
							{0,0,1}
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


