#version 330 core

layout ( location = 0 ) in vec3 position;

void main()
{
	gl_Position = vec4(0, position.y, 0, 1.0);
}