
#include "../DefaultCameraAndOtherConfig.hpp"

namespace SimpleDrawMultiIndirectBuffer {
int main() {
	DefaultsSetup();
	
	const int objectsToRender = 10;
    
	// Load shader
    gl::Shader ourShader;
	ourShader.Load(
			"../samples/SimpleDrawMultiIndirectBuffer/vertex.glsl",
			NULL,
			"../samples/SimpleDrawMultiIndirectBuffer/fragment.glsl");
    
	// Generate vertex data
    gl::VBO vbo(3*sizeof(float)+2*sizeof(uint16_t)+4*sizeof(uint8_t), gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	{
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
	}
    vbo.Generate();
	GL_CHECK_PUSH_ERROR;
	
	// Generate element buffer
	gl::VBO element(4, gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	{
		auto buf = element.Buffer<gl::Atr<uint32_t, 1>>();
		buf.At<0>(0) = 0;
		buf.At<0>(1) = 1;
		buf.At<0>(2) = 2;
		buf.At<0>(3) = 1;
		buf.At<0>(4) = 2;
		buf.At<0>(5) = 3;
	}
    element.Generate();
	GL_CHECK_PUSH_ERROR;
	
	// Init instance data buffer
	gl::VBO instanceData(64, gl::ARRAY_BUFFER, gl::DYNAMIC_DRAW);
	{
		auto buf = instanceData.Buffer<gl::Atr<glm::mat4, 1>>();
		for(int i=0; i<objectsToRender; ++i) {
			buf.At<0>(i) = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)), glm::vec3(i,i-5,i));
		}
	}
	instanceData.Generate();
	GL_CHECK_PUSH_ERROR;
	
	// Init indirect draw buffer
	struct DrawElementsIndirectCommand {
		uint32_t count;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t baseVertex;
		uint32_t baseInstance;
	};
	gl::VBO indirectDrawBuffer(sizeof(DrawElementsIndirectCommand), gl::DRAW_INDIRECT_BUFFER, gl::DYNAMIC_DRAW);
	{
		auto buf = indirectDrawBuffer.Buffer<gl::Atr<DrawElementsIndirectCommand, 1>>();
		for(int i=0; i<objectsToRender; ++i) {
			auto& c = buf.At<0>(i);
			c.instanceCount = 1;
			c.baseVertex = 0;
			c.baseInstance = i;
			
			c.count = ((rand()%3)%2)+1;
			c.firstIndex = c.count==1? rand()%2 : 0;
			c.count *= 3;
			c.firstIndex *= 3;
		}
	}
	indirectDrawBuffer.Generate();
	GL_CHECK_PUSH_ERROR;
	
    
	// Initiate VAO with VBO attributes
    gl::VAO vao(gl::TRIANGLES);
	GL_CHECK_PUSH_ERROR;
	
	// Set vertex attributes
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("position"), 3, gl::FLOAT, false, 0);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("uv"), 2, gl::UNSIGNED_SHORT, true, 12);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4, gl::UNSIGNED_BYTE, true, 16);
	GL_CHECK_PUSH_ERROR;
	
	// set instance data (model matrix)
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+0, 4, gl::FLOAT, false,  0, 1);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+1, 4, gl::FLOAT, false, 16, 1);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+2, 4, gl::FLOAT, false, 32, 1);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+3, 4, gl::FLOAT, false, 48, 1);
	GL_CHECK_PUSH_ERROR;
	
	// Set element attributes
	vao.BindElementBuffer(element, gl::UNSIGNED_INT);
	
	// Set indirect buffer
	vao.BindIndirectBuffer(indirectDrawBuffer);
	
    
	// Load texture
	gl::Texture texture;
    texture.Load("../samples/Simple/image.jpg", false, 4);
	
	// Get uniform locations
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
        
		// Set shader uniform matrices
        ourShader.SetMat4(viewLoc, view);
        ourShader.SetMat4(projLoc, projection);
		
		// Draw VAO
		vao.DrawMultiElementsIndirect(NULL, objectsToRender);
		
		DefaultIterationEnd();
    }
	
	GL_CHECK_PUSH_ERROR;
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}


