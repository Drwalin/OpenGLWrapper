#version 430 core

in vec2 out_uv;
in vec4 out_color;
in vec4 out_normal;
in vec4 out_pos;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec4 lightDir;
uniform vec2 winDim;

void main() {
	vec4 tex_color = texture(tex, out_uv);
	vec4 col = tex_color * out_color;
	
 	float d1 = dot(-out_normal.xyz, lightDir.xyz);
	if(d1 < 0.4) {
		float d = d1*5;
		d += 5;
		d /= 6;
		d*=d;
		d/=5;
		d1 =d;
	}
	
	FragColor = vec4((col * d1).xyz, 1);
}


