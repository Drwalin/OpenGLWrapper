
#include "../DefaultCameraAndOtherConfig.hpp"

namespace Texture {
int main() {
	DefaultsSetup();
    
	// Load shader
    gl::Shader ourShader;
	ourShader.Load(
			"../samples/Texture/vertex.glsl",
			NULL,
			"../samples/Texture/fragment.glsl");
    
	// Generate vertex data
    gl::VBO vbo(3*sizeof(float)+2*sizeof(uint16_t)+4*sizeof(uint8_t), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
    auto buf = vbo.Buffer<gl::Atr<float, 3>, gl::Atr<short,2>, gl::Atr<uint8_t,4>>();
    for(int i = 0; i < 4; ++i) {
		buf.At<0>(i, 0) = (i>>0)&1;
		buf.At<0>(i, 1) = (i>>2)&1;
		buf.At<0>(i, 2) = (i>>1)&1;
		buf.At<1>(i, 0) = ((i>>0)&1)*(65535);
		buf.At<1>(i, 1) = ((i>>1)&1)*(65535);
		
		buf.At<2>(i, 0) = 
		buf.At<2>(i, 1) = 
		buf.At<2>(i, 2) = 
		buf.At<2>(i, 3) = 255;
	}
	// Generate VBO from vertex data
    vbo.Generate();
    
	// Initiate VAO with VBO attributes
    gl::VAO vao(gl::TRIANGLE_STRIP);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3,
			gl::FLOAT, false, 0);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("uv"), 2,
			gl::UNSIGNED_SHORT, true, 12);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4,
			gl::UNSIGNED_BYTE, true, 16);
    
	// Load texture
	gl::Texture texture;
    texture.Load("../samples/Simple/image.jpg", false, 4);
	
	// Get uniform locations
	int modelLoc = ourShader.GetUniformLocation("model");
	int viewLoc = ourShader.GetUniformLocation("view");
	int projLoc = ourShader.GetUniformLocation("projection");
	int texLoc = ourShader.GetUniformLocation("tex");
	
	// Init camera position
    camera.ProcessMouseMovement(150, -200);
	
    while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
        
		// Use shader
        ourShader.Use();
		
		// Set texture
		ourShader.SetTexture(texLoc, &texture, 0);
        
		// Calculate projection matrix
        glm::mat4 projection = glm::perspective(45.0f,
				(float)gl::openGL.GetWidth()/(float)gl::openGL.GetHeight(), 0.1f,
				10000.0f);
        
		// Calculate view matrix
        glm::mat4 view = camera.getViewMatrix();
		
		// Calulate model matrix
		glm::mat4 model = glm::translate(
									glm::scale(glm::mat4(1.0f), glm::vec3(10)),
								glm::vec3(0.0f,-1.0f,0.0f));
        
		// Set shader uniform matrices
        ourShader.SetMat4(viewLoc, view);
        ourShader.SetMat4(projLoc, projection);
		ourShader.SetMat4(modelLoc, model);
		
		// Draw VAO
		vao.Draw();//0, 36);
        
		DefaultIterationEnd();
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}

