#version 430 core

in vec2 UV;
in vec4 COLOR;
in uint TEXTURE_ID;
in vec4 MODEL_COLOR;

out vec4 FragColor;

uniform sampler2D ourTexture1;

void main() {
	FragColor = COLOR * MODEL_COLOR;
	FragColor.z = 1;
}

