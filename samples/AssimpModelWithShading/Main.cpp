
#include "../DefaultCameraAndOtherConfig.hpp"
#include "openglwrapper/AssimpLoader.h"
#include "openglwrapper/VAO.h"
#include "openglwrapper/VBO.h"

#include <cstring>

namespace AssimpModelWithShading {
	
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
			"../samples/AssimpModelWithShading/vertex.glsl",
			NULL,
			"../samples/AssimpModelWithShading/fragment.glsl");
	
	// Load model
	gl::AssimpLoader l;
	l.Load("../samples/Monkey.fbx");
	
	// Prepare VBOs
	gl::VBO pos(sizeof(glm::vec3), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	gl::VBO uv(sizeof(glm::vec2), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	gl::VBO color(sizeof(glm::vec4), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	gl::VBO normal(sizeof(glm::vec3), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	gl::VBO indices(4, gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	
	// Copy data from model loader do VBOs
	Copy(pos, l.pos);
	Copy(uv, l.uv);
	Copy(color, l.color);
	Copy(normal, l.norm);
	Copy(indices, l.indices);
	
	// Generate VBOs
	pos.Generate();
	uv.Generate();
	color.Generate();
	normal.Generate();
	indices.Generate();
    
	// Initiate VAO with VBO attributes
    gl::VAO vao(gl::TRIANGLES);
	vao.SetAttribPointer(uv, ourShader.GetAttributeLocation("uv"), 2, gl::FLOAT, false, 0);
	vao.SetAttribPointer(color, ourShader.GetAttributeLocation("color"), 4, gl::FLOAT, false, 0);
	vao.SetAttribPointer(normal, ourShader.GetAttributeLocation("normal"), 3, gl::FLOAT, false, 0);
	vao.SetAttribPointer(pos, ourShader.GetAttributeLocation("pos"), 3, gl::FLOAT, false, 0);
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
// 		}
        
		DefaultIterationEnd();
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}


