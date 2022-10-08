#version 430 core

struct IndirectDrawData {
	uint count;
	uint instanceCount;
	uint firstIndex;
	int baseVertex;
	uint baseInstance;
};

layout (std430) buffer indirectDraw
{
	IndirectDrawData indirect[];
};

struct ObjectInfo {
	mat4 model;
	mat4 bones[64];
	uint textures[32];
	uint componentsCount;
};

layout (std430) buffer objectInfo
{
	ObjectInfo objects[];
};

layout (std430) buffer atomicCount {
	uint count[];
};

layout (local_size_x = 64, local_size_y = 16) in;


void main() {
	uint id = gl_GlobalInvocationID.x;
	uint indirectStart = atomicAdd(count[0], objects[id].componentsCount);
}

