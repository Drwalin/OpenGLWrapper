#version 330 core

layout ( location = 0 ) in vec3 position;

void main() {
	gl_Position = vec4((gl_InstanceID%1000)-1000,
			position.y,
			(gl_InstanceID/1000)-1000,
			1.0);
}

