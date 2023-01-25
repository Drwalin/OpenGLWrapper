#version 430 core

in vec2 out_uv;
in vec4 out_color;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
// 	vec4 tcol = texture(tex, vec2(0.5, 0.5));
	vec4 tcol = texture(tex, out_uv);
// 	FragColor = vec4((tcol * out_color).xyz, 1);
	FragColor = vec4(tcol.xyz, 1);
// 	FragColor.zx = out_uv;
	FragColor *= out_color;
}


