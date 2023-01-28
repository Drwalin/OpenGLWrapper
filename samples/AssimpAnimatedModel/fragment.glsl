#version 430 core

in vec4 out_color;
in vec4 out_normal;

out vec4 FragColor;

uniform vec4 lightDir;

void main() {
 	float d1 = dot(-out_normal.xyz, lightDir.xyz);
	if(d1 < 0.4) {
		float d = d1*5;
		d += 5;
		d /= 6;
		d*=d;
		d/=5;
		d1 =d;
	}
	
	FragColor = vec4((out_color * d1).xyz, 1);
}


