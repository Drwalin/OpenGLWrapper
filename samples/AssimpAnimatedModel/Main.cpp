
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
	
	
	
	
		// Generate vertex data
		gl::VBO vbo2(3*sizeof(float)+2*sizeof(uint16_t)+11*sizeof(uint8_t), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
		{
			auto buf = vbo2.Buffer<gl::Atr<float, 3>, gl::Atr<uint8_t,4>, gl::Atr<int8_t,3>, gl::Atr<uint8_t,4>, gl::Atr<uint8_t,4>>();
			for(int i = 0; i < 6; ++i) {
				buf.At<0>(i, 0) = 0;
				buf.At<0>(i, 1) = i;
				buf.At<0>(i, 2) = 0;
				buf.At<1>(i, 0) = 255;
				buf.At<1>(i, 1) = 255;
				buf.At<1>(i, 2) = 255;
				buf.At<1>(i, 3) = 255;
				buf.At<2>(i, 0) = 127;
				buf.At<2>(i, 1) = 0;
				buf.At<2>(i, 2) = 0;
				
				for(int j=0; j<4; ++j) {
					buf.At<3>(i, j) = 0;
					buf.At<4>(i, j) = 0;
				}
				buf.At<3>(i, 0) = 255;
				buf.At<4>(i, 0) = i;
			}
		}
		vbo2.Generate();
	
		gl::VAO vao2(gl::TRIANGLE_FAN);
		vao2.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("pos"), 3, gl::FLOAT, false, 0);
		vao2.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("color"), 4, gl::UNSIGNED_BYTE, true, 20);
		vao2.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("normal"), 3, gl::BYTE, true, 24);
		vao2.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("weights"), 4, gl::UNSIGNED_BYTE, true, 28);
		vao2.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("bones"), 4, gl::UNSIGNED_BYTE, false, 32);
	
		gl::VAO vao3(gl::POINTS);
		vao3.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("pos"), 3, gl::FLOAT, false, 0);
		vao3.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("color"), 4, gl::UNSIGNED_BYTE, true, 20);
		vao3.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("normal"), 3, gl::BYTE, true, 24);
		vao3.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("weights"), 4, gl::UNSIGNED_BYTE, true, 28);
		vao3.SetAttribPointer(vbo2, ourShader.GetAttributeLocation("bones"), 4, gl::UNSIGNED_BYTE, false, 32);
	
	
	
	// Get uniform locations
	int modelLoc = ourShader.GetUniformLocation("model");
	int viewLoc = ourShader.GetUniformLocation("view");
	int projLoc = ourShader.GetUniformLocation("projection");
	int lightDirLoc = ourShader.GetUniformLocation("lightDir");
	int bonesMatLoc = ourShader.GetUniformLocation("bonesMat");
	int useBonesLoc = ourShader.GetUniformLocation("useBones");
	
	bool useBones = true;
	bool drawModel = true;
	
	glPointSize(10.0f);
	
	// Init camera position
//     camera.ProcessMouseMovement(150, -200);
	
    while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
		
		if(WasPressed(GLFW_KEY_O))
			drawModel = !drawModel;
		
		if(WasPressed(GLFW_KEY_P))
			useBones = !useBones;
        
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
// 		matrices.clear();
		l.animations[0]->GetModelBoneMatrices(matrices, lastFrame, true);
// 		l.GetModelBoneMatrices(l.animations[0], l.meshes[0]->skeleton, matrices, lastFrame, true);
		ourShader.SetMat4(bonesMatLoc, matrices);
		
		// Calulate model matrix
		glm::mat4 model = glm::mat4(1);

		// Set shader uniform model matrice
		ourShader.SetMat4(modelLoc, model);

		if(useBones) {
			ourShader.SetBool(useBonesLoc, true);
		} else {
			ourShader.SetBool(useBonesLoc, false);
		}

		if(drawModel) {
			// Draw VAO
			vao.Draw();
		} else {
			vao2.Draw();
			vao3.Draw();
		}
			
		DefaultIterationEnd();
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}


