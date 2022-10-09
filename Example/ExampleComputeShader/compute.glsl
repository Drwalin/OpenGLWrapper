#version 430 core

struct IndirectDrawData {
	uint count;
	uint instanceCount;
	uint firstIndex;
	int baseVertex;
	uint baseInstance;
	int a, b, c;
};

layout ( std430, binding=4 ) buffer indirectDraw {
	IndirectDrawData indirect[];
};

struct ObjectInfo {
	vec2 pos;
	vec2 uv;
	int counts;
	int padding1;
	int elems[32];
	int pad[26];
};

layout ( std430, binding=5 ) buffer objectInfo {
	ObjectInfo objects[];
};

layout ( std430, binding=6 ) buffer atomicCount {
	uint count[];
};

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

uniform int objectsCount;

void main() {
	uint id = gl_GlobalInvocationID.x>>5;//(gl_GlobalInvocationID.x*1024) + gl_LocalInvocationID.x;
	if(id < objectsCount) {
		uint i = gl_GlobalInvocationID.x&31;
		if(i < objects[id].counts) {
			uint indirectStart = atomicAdd(count[0], 1);
			uint j = indirectStart;
			indirect[j].count = 1234;
			indirect[j].instanceCount = 1;
			indirect[j].firstIndex = objects[id].elems[i];
			indirect[j].baseVertex = 0;
			indirect[j].baseInstance = id;
			uint V = atomicAdd(count[16], 1);
			if(V+1 == 0)
				atomicAdd(count[17], 1);
		}
	}
	atomicMax(count[5], gl_GlobalInvocationID.x);
	atomicMax(count[6], gl_GlobalInvocationID.y);
	atomicMax(count[7], gl_GlobalInvocationID.z);
	atomicMax(count[8],  gl_LocalInvocationID.x);
	atomicMax(count[9],  gl_LocalInvocationID.y);
	atomicMax(count[10], gl_LocalInvocationID.z);
}

