#version 430 core

in vec3 pos;
in vec4 color;
in vec3 normal;
in vec4 weights;
in ivec4 bones;

const int MAX_BONES = 100;
uniform mat4 bonesMat[MAX_BONES];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 out_color;
out vec4 out_normal;
out vec4 out_pos;

vec4 Mult(vec4 point) {
// 	return (
// 		  bonesMat[bones.x] * weights.x
// 		+ bonesMat[bones.y] * weights.y
// 		+ bonesMat[bones.z] * weights.z
// 		+ bonesMat[bones.w] * weights.w
// 		) * point;
	
	return
		  (bonesMat[bones.x] * point) * weights.x
		+ (bonesMat[bones.y] * point) * weights.y
		+ (bonesMat[bones.z] * point) * weights.z
		+ (bonesMat[bones.w] * point) * weights.w;
}

void main() {
	
	vec4 vp = Mult(vec4(pos, 1));
	float t = 0.65;
	vp = vp * t + vec4(pos,1) * (1.0f-t);
	vp.w = 1;
	vec4 pp = model * vp;
	vec4 P = projection * view * pp;
	out_pos = P;
	gl_Position = P;
	out_color = color;
	out_normal = model*Mult(vec4(normal,1))-(model*Mult(vec4(0,0,0,1)));
	out_normal /= length(out_normal); 
}

