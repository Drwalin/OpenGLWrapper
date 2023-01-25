#version 430 core

layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 uv;
layout ( location = 2 ) in vec4 color;
layout ( location = 3 ) in mat4 model;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 out_uv;
out vec4 out_color;

void main() {
	gl_Position = projection * view * model * vec4(position,1);
	out_uv = uv;
	out_color = color;
}

