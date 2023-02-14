
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "Camera.hpp"

namespace SimpleCompute {
	int main();
}

namespace SimpleDrawMultiIndirectBuffer {
	int main();
}

namespace DrawMultiTestPerformanceVsInstanced {
	int main();
}

namespace Texture {
	int main();
}

namespace Simple {
	int main();
}

namespace ComplexCompute {
	int main();
}

namespace Particles_1 {
	int main();
}

namespace CircleLine {
	int main();
}

namespace AssimpModelWithShading {
	int main();
}

namespace AssimpAniamatedModel {
	int main();
}

namespace CameraFBO {
	int main();
}



struct Entry {
	const char* name;
	int (*entry)();
};

const Entry entries[] = {
	{
		"simple",
		Simple::main
	},
	{
		"texture",
		Texture::main
	},
	{
		"simple_compute",
		SimpleCompute::main
	},
	{
		"simple_draw_multi_indirect_buffer",
		SimpleDrawMultiIndirectBuffer::main
	},
	{
		"assimp_model_with_shading",
		AssimpModelWithShading::main
	},
	{
		"assim_animated_model",
		AssimpAniamatedModel::main
	},
	{
		"complex_compute",
		ComplexCompute::main,
	},
	{
		"particles_1",
		Particles_1::main
	},
	{
		"cirlce_line",
		CircleLine::main
	},
	{
		"draw_multi_test_performance_vs_instanced",
		DrawMultiTestPerformanceVsInstanced::main
	},
	{
		"camera_fbo",
		CameraFBO::main
	}
};

int main(int argc, char** argv) {
	int elems_count = sizeof(entries)/sizeof(Entry);
	
	if(argc > 1) {
		for(int i=0; i<elems_count; ++i) {
			if(strcmp(entries[i].name, argv[1]) == 0) {
				return entries[i].entry();
			}
		}
		
		char* end;
		int id = strtol(argv[1], &end, 10);
		if(!errno) {
			if(id >= 1 && id <= elems_count) {
				return entries[id-1].entry();
			}
		}
		
		printf("There is no sample with name: %s\n", argv[1]);
	}
	
	printf("Available samples:\n");
	for(int i=0; i<sizeof(entries)/sizeof(Entry); ++i) {
		printf("    [%3i] %s\n", i+1, entries[i].name);
	}
	printf("\nUsage:\n");
	printf("%s SAMPLE_NAME\n", argv[0]);
	
	return 1;
}

