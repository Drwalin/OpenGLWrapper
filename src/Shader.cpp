/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021-2023 Marek Zalewski aka Drwalin
 *
 *  OpenGLWrapper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenGLWrapper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../include/openglwrapper/Shader.hpp"
#include "../include/openglwrapper/Texture.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <cstdio>

namespace gl {

unsigned Shader::currentProgram = 0;

int Shader::Load(const char* vertexPath, const char* geometryPath,
		const char* fragmentPath) {
	Destroy();
	
	unsigned vertex = Shader::CompileShaderObject(vertexPath, gl::VERTEX_SHADER, "VERTEX");
	unsigned geometry = Shader::CompileShaderObject(geometryPath, gl::GEOMETRY_SHADER, "GEOMETRY");
	unsigned fragment = Shader::CompileShaderObject(fragmentPath, gl::FRAGMENT_SHADER, "FRAGMENT");
	
	program = glCreateProgram();
	if(geometry)
		glAttachShader(program, geometry);
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	
	if(vertex)
		glDeleteShader(vertex);
	if(geometry)
		glDeleteShader(geometry);
	if(fragment)
		glDeleteShader(fragment);
	
	return CheckBuildStatus();
}

int Shader::Load(const char* computePath) {
	Destroy();
	
	unsigned compute = Shader::CompileShaderObject(computePath, gl::COMPUTE_SHADER, "COMPUTE");
	
	program = glCreateProgram();
	glAttachShader(program, compute);
	glLinkProgram(program);
	
	if(compute)
		glDeleteShader(compute);
	
	int ret = CheckBuildStatus();
	if(ret == 0) {
		glGetProgramiv(program, GL_COMPUTE_WORK_GROUP_SIZE, workgroupSize);
	}
	return ret;
}

void Shader::Dispatch(uint32_t numGroupsX, uint32_t numGroupsY,
		uint32_t numGroupsZ) {
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

void Shader::DispatchRoundGroupNumbers(uint32_t numGroupsX, uint32_t numGroupsY,
		uint32_t numGroupsZ) {
	glDispatchCompute(
			this->workgroupSize[0]*((numGroupsX-1+this->workgroupSize[0])/this->workgroupSize[0]),
			this->workgroupSize[1]*((numGroupsY-1+this->workgroupSize[1])/this->workgroupSize[1]),
			this->workgroupSize[2]*((numGroupsZ-1+this->workgroupSize[2])/this->workgroupSize[2])
			);
}

unsigned Shader::CheckBuildStatus() {
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success) {
		char infoLog[512];
		int size;
		glGetProgramInfoLog(program, 512, &size, infoLog);
		printf("\n ERROR::SHADER::PROGRAM::LINKING_FAILED\n %s", infoLog);
		return 3;
	}
	return 0;
}

unsigned Shader::CompileShaderObject(const char* fileName, gl::ShaderType type, const char* shaderStrType) {
	char* code = NULL;
	
	std::ifstream file(fileName, std::ios::binary);
	if(!file.good())
		return 0;
	
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::end);
	fileSize = (size_t)file.tellg() - fileSize;
	file.seekg(0, std::ios::beg);
	
	code = (char*)malloc(fileSize+1);
	file.read(code, fileSize);
	code[fileSize] = 0;
	
	file.close();
	
	unsigned shaderId = Shader::CompileGLSL(code, type, shaderStrType);
	free(code);
	return shaderId;
}

unsigned Shader::CompileGLSL(const char* code, gl::ShaderType type,
		const char* shaderStrType) {
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
			printf("\n ERROR::SHADER::%s::COMPILATION_FAILED\n %s",
					shaderStrType, infoLog);
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
	int x=0;
	for(char const* i = code; *i; ++i) {
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
	int v = glGetAttribLocation(program, name);
	return v;
}

int Shader::GetUniformLocation(std::string name) const {
	return GetUniformLocation(name.c_str());
}

int Shader::GetAttributeLocation(std::string name) const {
	return GetAttributeLocation(name.c_str());
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
	glProgramUniform1i(program, location, value);
// 	Use();
// 	glUniform1i(location, value); 
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

void Shader::SetVec2(int location, const std::vector<glm::vec2>& array) {
	Use();
	glUniform2fv(location, array.size(), (float*)&array.front());
}

void Shader::SetVec3(int location, const std::vector<glm::vec3>& array) {
	Use();
	glUniform3fv(location, array.size(), (float*)&array.front());
}

void Shader::SetVec4(int location, const std::vector<glm::vec4>& array) {
	Use();
	glUniform4fv(location, array.size(), (float*)&array.front());
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
	glUniformMatrix4fv(location, array.size()*16*4, GL_FALSE,
			(float*)&array.front());
}

void Shader::Destroy() {
	if(currentProgram == program)
		currentProgram = 0;
	if(program) {
		glUseProgram(0);
		glDeleteProgram(program);
	}
	program = 0;
}

Shader::Shader() {
	program = 0;
}

Shader::~Shader() {
	Destroy();
}

}

