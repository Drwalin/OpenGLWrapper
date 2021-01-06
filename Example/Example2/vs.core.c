#version 330 core

in vec3 position;
in vec3 startVelocity;
in float lifeTime;
in float lifeOffset;
in vec4 color;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 acceleration;

out vec3 a, b, c, d;
out float life1;
out vec4 col1;

vec3 GetVelocity(float t) {
	return startVelocity + acceleration*t;
}

vec3 GetPosition(float t) {
	return position + (startVelocity + 0.5*acceleration*t)*t;
}

vec2 Rotate(vec2 v) {
	return vec2(v.y, -v.x);
}

vec3 Perpendicular(vec3 v) {
	vec3 ret;
	ret.xy = Rotate(v.xy);
	ret.xz = Rotate(vec2(ret.x, v.z));
	return ret;
}

void main() {
	float currentTime = mod(lifeOffset+time, lifeTime);
	life1 = currentTime / lifeTime;
	
	vec3 curr = GetPosition(currentTime);
	vec3 next = GetPosition(currentTime+0.05);
	vec3 dir = next - curr;
	dir = dir * (0.2 / length(dir));
	vec3 perp = Perpendicular(dir) * 0.2;
	vec3 mid = curr + (dir * 0.5);
	next = curr + dir;
	
	a = curr;
	b = mid + perp;
	c = mid - perp;
	d = next;
	
	col1 = color;
	gl_Position = vec4(curr, 1);/*
		(projection * view) *
		vec4(position +
		startVelocity*currentTime +
		0.5*acceleration*currentTime*currentTime, 1);*/
}

