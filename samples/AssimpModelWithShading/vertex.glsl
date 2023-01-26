#version 430 core

layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec2 uv;
layout ( location = 2 ) in vec4 color;
layout ( location = 3 ) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 out_uv;
out vec4 out_color;
out vec4 out_normal;

void main() {
	vec4 pp = model * vec4(pos, 1);
	gl_Position = projection * view * pp;
	out_uv = uv;
	out_color = color;
	out_normal = model*vec4(normal,1)-(model*vec4(0,0,0,1));
	out_normal /= length(out_normal); 
}

