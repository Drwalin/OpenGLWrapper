#version 430 core

in vec4 out_color;
in vec4 out_pos;
in vec2 out_uv;
in vec4 out_normal;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 NormalColor;

uniform vec4 lightDir;
uniform sampler2D tex;
uniform bool useTex;

void main() {
	NormalColor = vec4(((normalize(out_normal)+1)*0.5).xyz, 1);
	
 	float d1 = dot(-out_normal.xyz, lightDir.xyz);
	d1 = (d1+1.1f)/2.2f;
	
	vec4 c;
	
	if(useTex) {
		c = texture(tex, out_uv);
	} else {
		c = vec4((out_color * d1).xyz, 1);
	}
	
	FragColor = c;
}


