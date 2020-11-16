#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 28) out;

out vec2 texCoord;
out vec3 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void Vert(mat4 toview, vec3 p) {
	texCoord = p.xy;
	col = p;
	gl_Position = toview * vec4(gl_in[0].gl_Position.xyz + (p*0.25), 1);
	EmitVertex();
}

void main()
{
	vec3 grow  = vec3( 3.0, 0.0, 1.0 );
	float bup  = 4.0;
	
	mat4 toview = projection * view;// * model;
	
	float X = 3.0;
	vec4 p = toview * vec4(gl_in[0].gl_Position.xyz, 1);
	//if(p.z > 0)// && p.x>=-X && p.y>=-X && p.x<=X && p.y<=X)
	{
	Vert(toview, vec3(0,1,1));
	Vert(toview, vec3(1,1,1));
	Vert(toview, vec3(0,0,1));
	Vert(toview, vec3(1,0,1));
	Vert(toview, vec3(1,0,0));
	Vert(toview, vec3(1,1,1));
	Vert(toview, vec3(1,1,0));
	Vert(toview, vec3(0,1,1));
	Vert(toview, vec3(0,1,0));
	Vert(toview, vec3(0,0,1));
	Vert(toview, vec3(0,0,0));
	Vert(toview, vec3(1,0,0));
	Vert(toview, vec3(0,1,0));
	Vert(toview, vec3(1,1,0));
	
	
	/*
	Vert(toview, vec3(0,1,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,1,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(0,0,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,0,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,0,0)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,1,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,1,0)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(0,1,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(0,1,0)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(0,0,1)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(0,0,0)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,0,0)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(0,1,0)+vec3(0.5,0.5,0.5));
	Vert(toview, vec3(1,1,0)+vec3(0.5,0.5,0.5));
	*/
	}
	
	
	
	
	
	/*
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(0, 1, 0));
	
	Vert(toview, vec3(0, 0, 1));
	Vert(toview, vec3(1, 0, 1));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(0, 1, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(0, 1, 1));
	Vert(toview, vec3(0, 0, 1));
	
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(1, 0, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 0, 1));
	Vert(toview, vec3(0, 0, 1));
	
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(0, 1, 1));
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(0, 1, 0));
	
	Vert(toview, vec3(0, 0, 1));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(0, 1, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(0, 1, 1));
	Vert(toview, vec3(0, 0, 1));
	
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(1, 0, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 1));
	Vert(toview, vec3(0, 0, 1));
	
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(0, 1, 1));
	
	
	
	
	
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	
	Vert(toview, vec3(0, 0, 1));
	Vert(toview, vec3(1, 0, 1));
	Vert(toview, vec3(1, 1, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(0, 1, 1));
	
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 0, 1));
	
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	*/
	
	
	/*
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(0, 1, 0));
	
	Vert(toview, vec3(0, 0, 1));
	Vert(toview, vec3(1, 0, 1));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(0, 1, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(0, 1, 1));
	Vert(toview, vec3(0, 0, 1));
	
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(1, 0, 1));
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 0, 1));
	Vert(toview, vec3(0, 0, 1));
	
	Vert(toview, vec3(0, 1, 0));
	Vert(toview, vec3(1, 1, 0));
	Vert(toview, vec3(1, 1, 1));
	Vert(toview, vec3(0, 1, 1));
	*/
	EndPrimitive();
}






/*
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void Vert(mat4 toview, vec3 p) {
	texCoord = p.xy;
	gl_Position = toview * vec4(gl_in[0].gl_Position.xyz + p, 1);
	EmitVertex();
}

void main()
{
	vec3 grow  = vec3( 3.0, 0.0, 1.0 );
	float bup  = 4.0;
	
	mat4 toview = projection * view;// * model;
	
	Vert(toview, vec3(0, 0, 0));
	Vert(toview, vec3(1, 0, 0));
	Vert(toview, vec3(1, 1, 1));
	
	EndPrimitive();
}
*/