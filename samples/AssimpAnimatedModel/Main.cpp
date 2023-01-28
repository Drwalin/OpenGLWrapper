
#include "../DefaultCameraAndOtherConfig.hpp"
#include "openglwrapper/OpenGL.h"
#include "openglwrapper/basic_mesh_loader/AssimpLoader.hpp"
#include "openglwrapper/VAO.h"
#include "openglwrapper/VBO.h"
#include "openglwrapper/basic_mesh_loader/Value.hpp"

#include <cstring>

namespace AssimpAniamatedModel {
	
template<typename T>
void Copy(gl::VBO& vbo, std::vector<T>& v) {
	std::vector<uint8_t>& b = vbo.Buffer();
	b.resize(v.size()*sizeof(T));
	memcpy(&(b[0]), &(v[0]), b.size());
}
	
int main() {
	DefaultsSetup();
    
	// Load shader
    gl::Shader ourShader;
	ourShader.Load(
			"../samples/AssimpAnimatedModel/vertex.glsl",
			NULL,
			"../samples/AssimpAnimatedModel/fragment.glsl");
	
	// Load model
	gl::BasicMeshLoader::AssimpLoader l;
	l.Load("../samples/WobblyThing.fbx");
	
	std::shared_ptr<gl::BasicMeshLoader::Mesh> mesh = l.meshes[0];
	
	uint32_t stride
		= 3*sizeof(float) // pos
		+ 2*sizeof(float)    // uv
		+ 4*sizeof(uint8_t) // color
		+ 4*sizeof(uint8_t) // normal
		+ 4*(1 + 1); // bones + weights
	gl::VBO vbo(stride, gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	gl::VBO indices(4, gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	
	// Extract all desired attributes from mesh
	mesh->ExtractPos(0, vbo.Buffer(), 0, stride,
			gl::BasicMeshLoader::ConverterFloatPlain<float, 3>);
	
	mesh->ExtractUV(0, vbo.Buffer(), 12, stride,
			gl::BasicMeshLoader::ConverterFloatPlain<float, 2>);
	
	mesh->ExtractColor(0, vbo.Buffer(), 20, stride,
			gl::BasicMeshLoader::ConverterIntPlainClampScale<uint8_t, 255, 0, 255, 4>);
	
	mesh->ExtractNormal(0, vbo.Buffer(), 24, stride,
			gl::BasicMeshLoader::ConverterIntNormalized<uint8_t, 127, 3>);
	
	mesh->ExtractWeightsWithBones<uint8_t, uint8_t>(0, vbo.Buffer(), 28, 32, stride,
			gl::BasicMeshLoader::ConverterIntPlainClampScale<uint8_t, 255, 0, 255, 1>, 4);
	
	mesh->AppendIndices<uint32_t>(0, indices.Buffer());
	
	// Extract one frame
	std::vector<glm::mat4> matrices;
	l.animations[0]->GetModelBoneMatrices(matrices, 2, true);
	
	// Generate VBO & EBO
	vbo.Generate();
	indices.Generate();
	
	// Initiate VAO with VBO attributes
    gl::VAO vao(gl::TRIANGLES);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("pos"), 3, gl::FLOAT, false, 0);
// 	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("uv"), 2, gl::FLOAT, false, 12);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4, gl::UNSIGNED_BYTE, true, 20);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("normal"), 3, gl::BYTE, true, 24);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("weights"), 4, gl::UNSIGNED_BYTE, true, 28);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("bones"), 4, gl::UNSIGNED_BYTE, false, 32);
	vao.BindElementBuffer(indices, gl::UNSIGNED_INT);
	
	// Get uniform locations
	int modelLoc = ourShader.GetUniformLocation("model");
	int viewLoc = ourShader.GetUniformLocation("view");
	int projLoc = ourShader.GetUniformLocation("projection");
	int lightDirLoc = ourShader.GetUniformLocation("lightDir");
	int bonesMatLoc = ourShader.GetUniformLocation("bonesMat");
	
	// Init camera position
//     camera.ProcessMouseMovement(150, -200);
	
    while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
        
		// Use shader
        ourShader.Use();
		
		// Set light direction
		ourShader.SetVec4(lightDirLoc, glm::normalize(glm::rotate(glm::mat4(1),(lastFrame*0)/1.0f,glm::vec3(0,1,0))*glm::vec4(-1,-0.1,-1,1)));
        
		// Calculate projection matrix
        glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);
        
		// Calculate view matrix
        glm::mat4 view = camera.getViewMatrix();
		
		// Set shader uniform matrices
		ourShader.SetMat4(viewLoc, view);
		ourShader.SetMat4(projLoc, projection);
		
		// set animation (bone) matrices
		matrices.clear();
		l.animations[0]->GetModelBoneMatrices(matrices, lastFrame, true);
		ourShader.SetMat4(bonesMatLoc, matrices);
		
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
					glm::vec3(0.01)
				);

			// Set shader uniform model matrice
			ourShader.SetMat4(modelLoc, model);

			// Draw VAO
			vao.Draw();
// 		}
        
		DefaultIterationEnd();
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}


