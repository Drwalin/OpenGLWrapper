#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 grow  = vec3( 3.0, 0.0, 1.0 );
	float bup  = 4.0;
	
	mat4 toview = projection * view * model;
	
	for( int i = 0; i < 4; i += 1 )
	{
		texCoord = vec2( 1.0, float(i)*0.25 );
		gl_Position = toview * vec4( ( gl_in[0].gl_Position.xyz + vec3( grow.x, grow.y, grow.z*float(i*i)*1.5 ) )*0.1, 1.0 );
		EmitVertex();
		
		texCoord = vec2( 0.0, float(i)*0.25 );
		gl_Position = toview * vec4( ( gl_in[0].gl_Position.xyz + vec3( -grow.x, grow.y, grow.z*float(i*i)*1.5 ) )*0.1, 1.0 );
		EmitVertex();
		
		grow.y += bup;
		bup *= 0.75;
	}
	
	EndPrimitive();
}
