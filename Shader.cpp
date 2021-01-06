
#include "Shader.h"
#include "Texture.h"

#include <fstream>
#include <cstdio>

unsigned Shader::currentProgram = 0;

int Shader::Load(const char * vertexPath, const char * geometryPath,
		const char * fragmentPath) {
	if(this->program)
		return 311;
	
	const char* files[3] = {vertexPath, geometryPath, fragmentPath};
	char* code[3] = {NULL, NULL, NULL};
	
	for(int i=0; i<3; ++i) {
		if(files[i] == NULL)
			continue;
		std::ifstream file(files[i], std::ios::binary);
		if(file.good()) {
			size_t fileSize = file.tellg();
			file.seekg(0, std::ios::end);
			fileSize = (size_t)file.tellg() - fileSize;
			file.seekg(0, std::ios::beg);
			
			code[i] = (char*)malloc(fileSize+1);
			file.read(code[i], fileSize);
			code[i][fileSize] = 0;
			
			file.close();
		}
	}
	
	if(!code[0] || !code[2] || (geometryPath && !code[1])) {
		printf("\n ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ  v:%i - f:%i - g:%i ",
				(int)(bool)code[0], (int)(bool)code[2], (int)(bool)code[1]);
		for(int i=0; i<3; ++i)
			if(code[i])
				free(code[i]);
		return 1;
	}
	
	unsigned vertex, geometry, fragment;
	
	vertex = Shader::CompileProgram(code[0], GL_VERTEX_SHADER, "VERTEX");
	if(code[1])
		geometry = Shader::CompileProgram(code[1], GL_GEOMETRY_SHADER,
				"GEOMETRY");
	fragment = Shader::CompileProgram(code[2], GL_FRAGMENT_SHADER, "FRAGMENT");
	
	if(!vertex || !fragment || (geometryPath && !geometry)) {
		for(int i=0; i<3; ++i)
			if(code[i])
				free(code[i]);
		return 2;
	}
	
	program = glCreateProgram();
	if(geometryPath)
		glAttachShader(program, geometry);
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success) {
		char infoLog[512];
		int size;
		glGetProgramInfoLog(program, 512, &size, infoLog);
		printf("\n ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s", infoLog);
		for(int i=0; i<3; ++i)
			if(code[i])
				free(code[i]);
		return 3;
	}
	
	if(geometryPath)
		glDeleteShader(geometry);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	
	for(int i=0; i<3; ++i)
		if(code[i])
			free(code[i]);
	
	return 0;
}

unsigned Shader::CompileProgram(const char* code, unsigned type,
		const char* msg) {
	if(code) {
		int success;
		char infoLog[5120];
		unsigned program = glCreateShader(type);
		glShaderSource(program, 1, &code, NULL);
		glCompileShader(program);
		glGetShaderiv(program, GL_COMPILE_STATUS, &success);
		if(!success) {
			GLsizei size;
			glGetShaderInfoLog(program, 5120, &size, infoLog);
			printf("\n ERROR::SHADER::%s::COMPILATION_FAILED\n %s", msg,
					infoLog);
			PrintCode(code);
			glDeleteShader(program);
			return 0;
		}
		return program;
	}
	return 0;
}

void Shader::PrintCode(const char* code) {
	printf("\n Code: \n    1: ");
	int line = 1;
	char const*i = code;
	int x=0;
	for(; *i; ++i) {
		switch(*i) {
			case '\n':
				x = 0;
				++line;
				printf("\n%5i: ", line);
				break;
			case '\r':
				break;
			case '\t':
				for(int t=((x+4)>>2)<<2; x < t; ++x)
					putchar(' ');
				break;
			case 0:
				return;
			default:
				putchar(*i);
				++x;
		}
	}
}

void Shader::Use() {
	if(program) {
		if(currentProgram != program)
			glUseProgram(program);
		currentProgram = program;
	}
}

unsigned int Shader::GetProgram() {
	return program;
}

int Shader::GetUniformLocation(const char * name) const {
	return glGetUniformLocation(program, name);
}

int Shader::GetAttributeLocation(const char * name) const {
	return glGetAttribLocation(program, name);
}

void Shader::SetTexture(int location, class Texture* texture,
		unsigned textureId) {
	glActiveTexture(GL_TEXTURE0+textureId);
	texture->Bind();
	SetInt(location, textureId);
}

void Shader::SetBool(int location, bool value) {
	Use();
	glUniform1i(location, (int)value); 
}

void Shader::SetInt(int location, int value) {
	Use();
	glUniform1i(location, value); 
}

void Shader::SetInt(int location, const std::vector<int>& array) {
	Use();
	glUniform1uiv(location, array.size(), (unsigned int*)&array.front());
}

void Shader::SetInt(int location, const std::vector<unsigned int>& array) {
	Use();
	glUniform1uiv(location, array.size(), &array.front());
}

void Shader::SetFloat(int location, float value) { 
	Use();
	glUniform1f(location, value); 
}

void Shader::SetFloat(int location, const std::vector<float>& array) {
	Use();
	glUniform1fv(location, array.size(), &array.front());
}

void Shader::SetVec2(int location, const glm::vec2 &value) {
	Use();
	glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(int location, const glm::vec3 &value) {
	Use();
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(int location, const std::vector<glm::vec3>& array) {
	Use();
	glUniform3fv(location, array.size(), (float*)&array.front());
}

void Shader::SetVec4(int location, const glm::vec4 &value) {
	Use();
	glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::SetMat2(int location, const glm::mat2 &mat) {
	Use();
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMat3(int location, const glm::mat3 &mat) {
	Use();
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMat4(int location, const glm::mat4 &mat) {
	Use();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMat4(int location, const std::vector<glm::mat4>& array) {
	Use();
	glUniformMatrix4fv(location, array.size(), GL_FALSE,
			(float*)&array.front());
}

Shader::Shader() {
	program = 0;
}

Shader::~Shader() {
	if(program) {
		glUseProgram(0);
		glDeleteProgram(program);
	}
	program = 0;
}
