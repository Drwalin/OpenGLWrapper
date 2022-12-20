#version 430 core
layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout ( std430, binding = 4 ) buffer source {
	uint src[];
};
layout ( std430, binding = 5 ) buffer destiny {
	uint dst[];
};

uniform int objectsCount;

void main() {
	uint id = gl_GlobalInvocationID.x;
	if(id >= objectsCount)
		return;
	uint sum = 0, base = src[id];
	for(int i=0; i<objectsCount; ++i)
		sum += src[i];
	dst[id] = sum * src[id];
}

