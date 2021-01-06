/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
	
	enum Type {
		Vertex = 0,
		Geometry = 1,
		Fragment = 2
		//Tesselation = 3,
		//Compute = 4
	};
	inline static const unsigned gl_types[] = {
		GL_VERTEX_SHADER,
		GL_GEOMETRY_SHADER,
		GL_FRAGMENT_SHADER
	};
	inline static const char* const gl_string_types[] = {
		"VERTEX",
		"GEOMETRY",
		"FRAGMENT"
	};
	
	
	int Load(const char* vertexPath, const char* geometryPath,
			const char* fragmentPath);		// return 0 if no errors
	void Use();
	unsigned GetProgram();
	
	int GetUniformLocation(const char* name) const;
	int GetAttributeLocation(const char* name) const;
	
	void SetTexture(int location, class Texture* texture, unsigned textureId);
	void SetBool(int location, bool value);
	void SetInt(int location, int value);
	void SetInt(int location, const std::vector<int>& array);
	void SetInt(int location, const std::vector<unsigned int>& array);
	void SetFloat(int location, float value);
	void SetFloat(int location, const std::vector<float>& array);
	void SetVec2(int location, const glm::vec2 &value);
	void SetVec3(int location, const glm::vec3 &value);
	void SetVec4(int location, const glm::vec4 &value);
	void SetVec2(int location, const std::vector<glm::vec2>& array);
	void SetVec3(int location, const std::vector<glm::vec3>& array);
	void SetVec4(int location, const std::vector<glm::vec4>& array);
	void SetMat2(int location, const glm::mat2 &mat);
	void SetMat3(int location, const glm::mat3 &mat);
	void SetMat4(int location, const glm::mat4 &mat);
	void SetMat4(int location, const std::vector<glm::mat4>& array);
	
	void Destroy();
	
	Shader();
	~Shader();
	
private:
	
	static unsigned currentProgram;
	
	static unsigned CompileShaderObject(const char* fileName, Type type);
	static unsigned CompileGLSL(const char* code, Type type);
	static void PrintCode(const char* code);
	
	unsigned int program;
};

#endif

