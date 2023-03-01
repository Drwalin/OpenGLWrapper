#version 430 core

in vec3 pos;
in vec2 uv;
in vec4 color;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 out_color;
out vec2 out_uv;;
out vec4 out_normal;
out vec4 out_pos;

void main() {
	vec4 vp = model * vec4(pos, 1);
	
	vp.w = 1;
	vec4 P = projection * view * vp;
	out_pos = P;
	gl_Position = P;
	out_color = color;
	out_normal = model*vec4(normal, 0);
	out_normal /= length(out_normal); 
	out_uv = uv;
}

