#version 330 core

in vec4 col;
in float life;

out vec4 FragColor;

uniform sampler2D ourTexture1;

vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec4 c = col;
	c.w = 1.0;//c.w * (1 - life);
	FragColor = c;//vec4(0.7, 0.6, 0, 1);
	//FragColor = vec4(hsv2rgb(vec3(gl_FragCoord.x/800.0, 1, gl_FragCoord.y/600.0)), 1);//vec4(gl_FragCoord.x/800.0, gl_FragCoord.y/600.0, 1, 1);
}

