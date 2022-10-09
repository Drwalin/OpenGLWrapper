#version 430 core

layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec2 uv;
layout ( location = 2 ) in vec4 color;

layout ( location = 3 ) in mat4 model;
layout ( location = 7 ) in uint objectId;
layout ( location = 8 ) in uint componentId;
layout ( location = 9 ) in uint textureId;
layout ( location = 10 ) in vec4 objectColor;

out vec2 UV;
out vec4 COLOR;
out uint TEXTURE_ID;
out vec4 MODEL_COLOR;

uniform mat4 viewProjection;

void main() {
	UV = uv;
	COLOR = color;
	TEXTURE_ID = textureId;
	MODEL_COLOR = objectColor;
	gl_Position = viewProjection * model * vec4(pos, 1);
}

