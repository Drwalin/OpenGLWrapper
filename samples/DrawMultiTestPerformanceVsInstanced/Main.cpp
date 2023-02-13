
#include "../DefaultCameraAndOtherConfig.hpp"
#include "openglwrapper/basic_mesh_loader/AssimpLoader.hpp"
#include "openglwrapper/basic_mesh_loader/Value.hpp"

#include <cstring>

namespace DrawMultiTestPerformanceVsInstanced {
	
template<typename T>
void Copy(gl::VBO& vbo, std::vector<T>& v) {
	std::vector<uint8_t>& b = vbo.Buffer();
	b.resize(v.size()*sizeof(T));
	memcpy(&(b[0]), &(v[0]), b.size());
}
	
int main() {
	DefaultsSetup();
    
	int MAX_OBJECTS = 1000*1000;
	const int objectsToRender = 1000;
	
	// Load shader
    gl::Shader ourShader;
	ourShader.Load(
			"../samples/DrawMultiTestPerformanceVsInstanced/vertex.glsl",
			NULL,
			"../samples/DrawMultiTestPerformanceVsInstanced/fragment.glsl");
	
	// Load model
	gl::BasicMeshLoader::AssimpLoader l;
	l.Load("../samples/Monkey.fbx");
	GL_CHECK_PUSH_ERROR;
	
	std::shared_ptr<gl::BasicMeshLoader::Mesh> mesh = l.meshes[0];
	GL_CHECK_PUSH_ERROR;
	
	uint32_t stride
		= 3*sizeof(float)
		+ 2*sizeof(float)
		+ 4*sizeof(uint8_t)
		+ 4*sizeof(uint8_t);
	gl::VBO vbo(stride, gl::ARRAY_BUFFER, gl::STATIC_DRAW);
	gl::VBO indices(4, gl::ELEMENT_ARRAY_BUFFER, gl::STATIC_DRAW);
	
	// Extract all desired attributes from mesh
	mesh->ExtractPos<float>(0, vbo.Buffer(), 0, stride,
			gl::BasicMeshLoader::ConverterFloatPlain<float, 3>);
	
	mesh->ExtractUV<float>(0, vbo.Buffer(), 12, stride,
			gl::BasicMeshLoader::ConverterFloatPlain<float, 2>);
	
	mesh->ExtractColor<uint8_t>(0, vbo.Buffer(), 20, stride,
			gl::BasicMeshLoader::ConverterIntPlainClampScale<uint8_t, 255, 0, 255, 4>);
	
	mesh->ExtractNormal(0, vbo.Buffer(), 24, stride,
			gl::BasicMeshLoader::ConverterIntNormalized<uint8_t, 127, 3>);
	
	mesh->AppendIndices<uint32_t>(0, indices.Buffer());
	
	
	
	
	
	// Generate VBO & EBO
	vbo.Generate();
	GL_CHECK_PUSH_ERROR;
	indices.Generate();
	GL_CHECK_PUSH_ERROR;
	
	
	
	
	
	
	// Init instance data buffer
	gl::VBO instanceData(64, gl::ARRAY_BUFFER, gl::DYNAMIC_DRAW);
	{
		auto buf = instanceData.Buffer<gl::Atr<glm::mat4, 1>>();
		int i=0;
		for(int y=0; y<100; ++y) {
			for(int x=0; x<100; ++x) {
				for(int z=0; z<100; ++z, ++i) {
					buf.At<0>(i) = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) * 1.5f);
				}
			}
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
		for(int i=0; i<MAX_OBJECTS; ++i) {
			auto& c = buf.At<0>(i);
			c.instanceCount = 0;
			c.baseVertex = 0;
			c.baseInstance = i;
			
			c.count = indices.Buffer().size()/4;
			c.firstIndex = 0;
		}
		for(int i=0; i<objectsToRender; ++i) {
			int I = ((uint64_t)i*(uint64_t)1296817)%(uint64_t)MAX_OBJECTS;
			auto& c = buf.At<0>(I);
			c.instanceCount = 1;
		}
	}
	indirectDrawBuffer.Generate();
	GL_CHECK_PUSH_ERROR;
	
	
	
	
	// Initiate VAO with VBO attributes
    gl::VAO vao(gl::TRIANGLES);
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("pos"), 3, gl::FLOAT, false, 0);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("uv"), 2, gl::FLOAT, false, 12);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("color"), 4, gl::UNSIGNED_BYTE, true, 20);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(vbo, ourShader.GetAttributeLocation("normal"), 3, gl::BYTE, true, 24);
	GL_CHECK_PUSH_ERROR;
	
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+0, 4, gl::FLOAT, false,  0, 1);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+1, 4, gl::FLOAT, false, 16, 1);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+2, 4, gl::FLOAT, false, 32, 1);
	GL_CHECK_PUSH_ERROR;
	vao.SetAttribPointer(instanceData, ourShader.GetAttributeLocation("model")+3, 4, gl::FLOAT, false, 48, 1);
	GL_CHECK_PUSH_ERROR;
	
	vao.BindElementBuffer(indices, gl::UNSIGNED_INT);
	vao.BindIndirectBuffer(indirectDrawBuffer);
    
	// Load texture
	gl::Texture texture;
    texture.Load("../samples/image.jpg", false, 4);
	
	// Get uniform locations
	int viewLoc = ourShader.GetUniformLocation("view");
	int projLoc = ourShader.GetUniformLocation("projection");
	int texLoc = ourShader.GetUniformLocation("tex");
	int lightDirLoc = ourShader.GetUniformLocation("lightDir");
	int renderTargetDimLoc = ourShader.GetUniformLocation("winDim");
	
	// Init camera position
//     camera.ProcessMouseMovement(150, -200);
	
	int frameCount = 0;
	float start = -1;
    while(!glfwWindowShouldClose(gl::openGL.window)) {
		DefaultIterationStart();
		if(start < 0)
			start = lastFrame;
		frameCount++;
        
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
		

		
		// Draw VAO
		vao.DrawMultiElementsIndirect(NULL, MAX_OBJECTS);
		// Draw VAO
// 		vao.SetInstances(objectsToRender);
// 		vao.Draw();

        
		DefaultIterationEnd();
		
		printf(" fps = %f vertices=%i, triangles=%i, instances=%i   => all triangles = %lli\n",
				((float)frameCount)/(glfwGetTime()-start),
				vbo.Buffer().size() / vbo.VertexSize(),
				indices.Buffer().size() / indices.VertexSize(),
				objectsToRender,
				(long long)indices.Buffer().size() / (long long)indices.VertexSize() * (long long)objectsToRender
				);
    }
	
	gl::openGL.Destroy();
	glfwTerminate();
	
    return EXIT_SUCCESS;
}
}


