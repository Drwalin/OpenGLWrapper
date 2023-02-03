#version 330 core

layout (points) in;
// layout (triangle_strip, max_vertices = 4) out;
layout (triangle_strip, max_vertices = 32) out;

in vec3 a[], b[], c[], d[], e[], f[];
in vec4 col1[];
in float life1[];

out float life;
out vec4 col;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform vec3 acceleration;

void Vert(mat4 toview, vec3 p) {
	gl_Position = toview * vec4(p, 1);//gl_in[0].gl_Position.xyz + (p*0.25), 1);
	EmitVertex();
}

void main() {
	mat4 toview = projection * view;
	
	col = col1[0];
	life = life1[0];
	
// 	Vert(toview, a[0]);
// 	Vert(toview, b[0]);
// 	Vert(toview, c[0]);
// 	Vert(toview, d[0]);
	
// 	Vert(toview, a[0]);
// 	Vert(toview, e[0]);
// 	Vert(toview, f[0]);
// 	Vert(toview, d[0]);
	
	
	
	
	Vert(toview, a[0]);
	Vert(toview, b[0]);
	Vert(toview, e[0]);
	EndPrimitive();
	
	Vert(toview, a[0]);
	Vert(toview, b[0]);
	Vert(toview, f[0]);
	EndPrimitive();
	
	Vert(toview, a[0]);
	Vert(toview, c[0]);
	Vert(toview, e[0]);
	EndPrimitive();
	
	Vert(toview, a[0]);
	Vert(toview, c[0]);
	Vert(toview, f[0]);
	EndPrimitive();
	
	
	Vert(toview, d[0]);
	Vert(toview, b[0]);
	Vert(toview, e[0]);
	EndPrimitive();
	
	Vert(toview, d[0]);
	Vert(toview, b[0]);
	Vert(toview, f[0]);
	EndPrimitive();
	
	Vert(toview, d[0]);
	Vert(toview, c[0]);
	Vert(toview, e[0]);
	EndPrimitive();
	
	Vert(toview, d[0]);
	Vert(toview, c[0]);
	Vert(toview, f[0]);
	EndPrimitive();
}

