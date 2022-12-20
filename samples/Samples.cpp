
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "Camera.hpp"

namespace SimpleCompute {
	int main();
}

namespace Simple {
	int main();
}

namespace ComplexCompute {
	int main();
}

namespace DrawMultiIndirectBuffer {
	int main();
}

namespace Particles_1 {
	int main();
}




struct Entry {
	const char* name;
	int (*entry)();
};

const Entry entries[] = {
	{
		"simple_compute",
		SimpleCompute::main
	},
	{
		"simple",
		Simple::main
	},
	{
		"complex_compute",
		ComplexCompute::main,
	},
	{
		"draw_multi_indirect_buffer",
		DrawMultiIndirectBuffer::main
	},
	{
		"particles_1",
		Particles_1::main
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

