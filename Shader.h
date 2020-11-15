#ifndef SHADER_H
#define SHADER_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	
    int Load(const char* vertexPath, const char* geometryPath, const char* fragmentPath);		// return 0 if no errors
    void Use();
    unsigned GetProgram();
	
	int GetUniformLocation(const char* name) const;
	int GetAttributeLocation(const char* name) const;
	
	void SetBool(const int location, bool value);
	void SetInt(const int location, int value);
	void SetInt(const int location, const std::vector<int>& array);
	void SetInt(const int location, const std::vector<unsigned int>& array);
	void SetFloat(const int location, float value);
	void SetFloat(const int location, const std::vector<float>& array);
	void SetVec2(const int location, const glm::vec2 &value);
	void SetVec3(const int location, const glm::vec3 &value);
	void SetVec3(const int location, const std::vector<glm::vec3>& array);
	void SetVec4(const int location, const glm::vec4 &value);
	void SetMat2(const int location, const glm::mat2 &mat);
	void SetMat3(const int location, const glm::mat3 &mat);
	void SetMat4(const int location, const glm::mat4 &mat);
	void SetMat4(const int location, const std::vector<glm::mat4>& array);
	
	Shader();
	~Shader();
	
private:
	
	static unsigned currentProgram;
	
	static unsigned CompileProgram(const char* code, unsigned type, const char* msg);
	static void PrintCode(const char* code);
	
    unsigned int program;
	
};

#endif
