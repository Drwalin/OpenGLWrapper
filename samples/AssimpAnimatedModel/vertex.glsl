#version 430 core

in vec3 pos;
in vec4 color;
in vec3 normal;
in vec4 weights;
in vec4 bones;

const int MAX_BONES = 100;
uniform mat4 bonesMat[MAX_BONES];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 out_color;
out vec4 out_normal;
out vec4 out_pos;

vec4 Mult(vec4 point) {
	return
		  ((bonesMat[int(bones[0])] * point) * weights[0])
		+ ((bonesMat[int(bones[1])] * point) * weights[1])
		+ ((bonesMat[int(bones[2])] * point) * weights[2])
		+ ((bonesMat[int(bones[3])] * point) * weights[3]);
}

void main() {
	vec4 vp = model * Mult(vec4(pos, 1));
	
	vp.w = 1;
	vec4 P = projection * view * vp;
	out_pos = P;
	gl_Position = P;
	out_color = color;
	out_normal = model*Mult(vec4(normal, 0));
	out_normal /= length(out_normal); 
}

