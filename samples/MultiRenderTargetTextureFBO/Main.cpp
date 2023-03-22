
#include "../DefaultCameraAndOtherConfig.hpp"
#include "../../include/openglwrapper/basic_mesh_loader/SimpleRender.hpp"
#include "openglwrapper/OpenGL.hpp"
#include "openglwrapper/Texture.hpp"
#include "openglwrapper/FBO.hpp"

#include <cstring>
#include <filesystem>

namespace MultiRenderTargetTextureFBO {

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
			"../../samples/MultiRenderTargetTextureFBO/vertex.glsl",
			"",
			"../../samples/MultiRenderTargetTextureFBO/fragment.glsl");
	
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
	
	// gen and setup FBO
	gl::Texture renderTextureColor, renderTextureNormal, renderTextureDepth;
	
	renderTextureColor.UpdateTextureData(NULL, gl::openGL.GetWidth(), gl::openGL.GetHeight(),
			false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)gl::RGBA, gl::RGBA, gl::UNSIGNED_BYTE);
	renderTextureColor.SetDefaultParamPixelartClampBorderNoMipmap();
	
	renderTextureNormal.UpdateTextureData(NULL, gl::openGL.GetWidth(), gl::openGL.GetHeight(),
			false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)gl::RGBA, gl::RGBA, gl::UNSIGNED_BYTE);
	renderTextureNormal.SetDefaultParamPixelartClampBorderNoMipmap();
	
	renderTextureDepth.UpdateTextureData(NULL, gl::openGL.GetWidth(), gl::openGL.GetHeight(),
			false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)GL_DEPTH24_STENCIL8,
			(gl::TextureDataFormat)GL_DEPTH_STENCIL, (gl::DataType)GL_UNSIGNED_INT_24_8);
	renderTextureColor.SetDefaultParamPixelartClampBorderNoMipmap();
	
	gl::FBO fbo;
	fbo.AttachTexture(&renderTextureColor, gl::ATTACHMENT_COLOR0, 0);
	fbo.AttachTexture(&renderTextureNormal, gl::ATTACHMENT_COLOR1, 1);
	fbo.AttachTexture(&renderTextureDepth, gl::ATTACHMENT_DEPTH_STENCIL, 0);
	
	
	GLenum Status = fbo.CheckStatus();
	if(Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	
	gl::Texture* drawingTexture = &renderTextureColor;
	Camera camera2;
	
	while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
		
		if(renderTextureColor.GetWidth() != gl::openGL.GetWidth() ||
				renderTextureColor.GetHeight() != gl::openGL.GetHeight()) {
			renderTextureColor.UpdateTextureData(NULL, gl::openGL.GetWidth(), gl::openGL.GetHeight(),
					false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)gl::RGBA, gl::RGBA, gl::UNSIGNED_BYTE);
// 			renderTextureColor.SetDefaultParamPixelartClampBorderNoMipmap();
			
			renderTextureNormal.UpdateTextureData(NULL, gl::openGL.GetWidth(), gl::openGL.GetHeight(),
					false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)gl::RGBA, gl::RGBA, gl::UNSIGNED_BYTE);
// 			renderTextureNormal.SetDefaultParamPixelartClampBorderNoMipmap();
			
			renderTextureDepth.UpdateTextureData(NULL, gl::openGL.GetWidth(), gl::openGL.GetHeight(),
					false, gl::TEXTURE_2D, (gl::TextureSizedInternalFormat)GL_DEPTH24_STENCIL8,
					(gl::TextureDataFormat)GL_DEPTH_STENCIL, (gl::DataType)GL_UNSIGNED_INT_24_8);
// 			renderTextureColor.SetDefaultParamPixelartClampBorderNoMipmap();
		}
		
		if(WasPressed(GLFW_KEY_1)) {
			drawingTexture = &renderTextureColor;
		} else if(WasPressed(GLFW_KEY_2)) {
			drawingTexture = &renderTextureNormal;
		} else if(WasPressed(GLFW_KEY_3)) {
			drawingTexture = &renderTextureDepth;
		}
		
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
		
		// Render everything for camera (FBO)
		{
			shader->Use();
			// Set up FBO
			fbo.Bind();
// 			GLenum es[] = {gl::ATTACHMENT_COLOR0, gl::ATTACHMENT_COLOR1};
// 			glDrawBuffers(2, es);
			shader->SetTexture(texLoc, NULL, 0);
			
			// Set Viewport
			fbo.SetClearColor({0.2,0.3,8,0.5});
			fbo.Clear(true, true);
			fbo.Viewport(0, 0, gl::openGL.GetWidth(), gl::openGL.GetHeight());
			
			// Calculate andset projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f,
						(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
						10000.0f));
			
			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera.getViewMatrix());
			
			
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
			
			// Set down FBO
			fbo.Unbind();
			gl::openGL.InitFrame();
		}
		
		// Render buffer to screen
		// Here should be done ie. post processing
		{
			shader->Use();
			// Calculate and set projection matrix
			shader->SetMat4(projLoc, glm::perspective(45.0f, 1.0f, 0.1f, 10000.0f));

			// Calculate and set view matrix
			shader->SetMat4(viewLoc, camera2.getViewMatrix());
			
			// Render FBO result
			gl::openGL.FaceCulling(true, true);
			shader->SetBool(useTexLoc, true);
			shader->SetTexture(texLoc, drawingTexture, 0);
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


