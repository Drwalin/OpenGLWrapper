#version 330 core

in vec2 position;

uniform vec2 scale;
uniform vec2 offset;
uniform vec2 viewport_size;

void main() {
	vec2 p = position;
	p.y /= viewport_size.y / viewport_size.x;
	gl_Position = vec4(p*scale - offset, 0, 1);
}

