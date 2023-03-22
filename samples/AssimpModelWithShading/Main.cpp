
#include "../DefaultCameraAndOtherConfig.hpp"
#include "openglwrapper/basic_mesh_loader/AssimpLoader.hpp"
#include "openglwrapper/basic_mesh_loader/Value.hpp"

#include <cstring>

namespace AssimpModelWithShading {
	
int main() {
	DefaultsSetup();
    
	// Load shader
    gl::Shader ourShader;
	ourShader.Load(
			"../samples/AssimpModelWithShading/vertex.glsl",
			"",
			"../samples/AssimpModelWithShading/fragment.glsl");
	
	// Load model
	gl::BasicMeshLoader::AssimpLoader l;
	l.Load("../samples/Monkey.fbx");
	
	std::shared_ptr<gl::BasicMeshLoader::Mesh> mesh = l.meshes[0];
	
	uint32_t stride
		= 3*sizeof(float)
		+ 2*sizeof(float)
		+ 4*sizeof(uint8_t)
		+ 4*sizeof(uint8_t);
	gl::VBO vbo(stride, gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	vbo.Init();
	gl::VBO indices(4, gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	indices.Init();
	
	// Extract all desired attributes from mesh
	std::vector<uint8_t> Vbo, Ebo;
	mesh->ExtractPos<float>(0, Vbo, 0, stride,
			gl::BasicMeshLoader::ConverterFloatPlain<float, 3>);
	
	mesh->ExtractUV<float>(0, Vbo, 12, stride,
			gl::BasicMeshLoader::ConverterFloatPlain<float, 2>);
	
	mesh->ExtractColor<uint8_t>(0, Vbo, 20, stride,
			gl::BasicMeshLoader::ConverterIntPlainClampScale<uint8_t, 255, 0, 255, 4>);
	
	mesh->ExtractNormal(0, Vbo, 24, stride,
			gl::BasicMeshLoader::ConverterIntNormalized<uint8_t, 127, 3>);
	
	mesh->AppendIndices<uint32_t>(0, Ebo);
	
	// Generate VBO & EBO
	vbo.Generate(Vbo);
	indices.Generate(Ebo);
	
	// Initiate VAO with VBO attributes
    gl::VAO vao(gl::TRIANGLES);
	vao.Init();
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("pos"), 3, gl::FLOAT, false, 0);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("uv"), 2, gl::FLOAT, false, 12);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4, gl::UNSIGNED_BYTE, true, 20);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("normal"), 3, gl::BYTE, true, 24);
	vao.BindElementBuffer(indices, gl::UNSIGNED_INT);
    
	// Load texture
	gl::Texture texture;
    texture.Load("../samples/image.jpg", false, 4);
	
	// Get uniform locations
	int modelLoc = ourShader.GetUniformLocation("model");
	int viewLoc = ourShader.GetUniformLocation("view");
	int projLoc = ourShader.GetUniformLocation("projection");
	int texLoc = ourShader.GetUniformLocation("tex");
	int lightDirLoc = ourShader.GetUniformLocation("lightDir");
	int renderTargetDimLoc = ourShader.GetUniformLocation("winDim");
	
	// Init camera position
//     camera.ProcessMouseMovement(150, -200);
	
    while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
        
		// Use shader
        ourShader.Use();
		
		// Set light direction
		ourShader.SetVec4(lightDirLoc, glm::normalize(glm::rotate(glm::mat4(1),(lastFrame)/1.0f,glm::vec3(0,1,0))*glm::vec4(-1,-0.1,-1,1)));
		
		// Set texture
		ourShader.SetTexture(texLoc, &texture, 0);
        
		// Calculate projection matrix
        glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);
        
		// Calculate view matrix
        glm::mat4 view = camera.getViewMatrix();
		
		// Set shader uniform matrices
		ourShader.SetMat4(viewLoc, view);
		ourShader.SetMat4(projLoc, projection);
		
		glm::vec2 winDim = {gl::openGL.GetWidth(), gl::openGL.GetHeight()};
		ourShader.SetVec2(renderTargetDimLoc, winDim);
		
		// Calulate model matrix
		glm::mat4 model = 
			glm::scale(
					glm::rotate(
						glm::rotate(
							glm::translate(
								glm::mat4(1.0f),
								glm::vec3(2, -1, 0)
							),
							-3.141592f/2,
							glm::vec3(0,1,0)
						),
						3.141592f/2.0f,
						glm::vec3(-1,0,0)
					),
					glm::vec3(1.0f)
				);

			// Set shader uniform model matrice
			ourShader.SetMat4(modelLoc, model);

			// Draw VAO
			vao.Draw();
// 			vao.Draw(0, 900);
// 			vao.DrawArrays(0, 900);
// 		}
        
		DefaultIterationEnd();
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}


