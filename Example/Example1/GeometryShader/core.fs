#version 330 core

in vec2 texCoord;
in vec3 col;

out vec4 FragColor;

uniform sampler2D ourTexture1;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
//	FragColor = vec4(0.5, 0.5, 0.3, 1);
	float t = 1;//sqrt(gl_FragCoord.w);
	FragColor = vec4(hsv2rgb(vec3(gl_FragCoord.x/800.0, 1, gl_FragCoord.y/600.0)), 1);//vec4(gl_FragCoord.x/800.0, gl_FragCoord.y/600.0, 1, 1);
//	FragColor = vec4(col, 1);
}

