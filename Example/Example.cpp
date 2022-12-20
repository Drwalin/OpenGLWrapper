
#include <cstring>
#include <cstdio>

#include "Camera.hpp"

namespace BasicCompute {
	int main();
}

namespace Basic {
	int main();
}

namespace ComplexCompute {
	int main();
}

namespace DrawMultiIndirectBuffer {
	int main();
}




struct Entry {
	const char* name;
	int (*entry)();
};

const Entry entries[] = {
	{
		"basic_compute",
		BasicCompute::main
	},
	{
		"basic",
		Basic::main
	},
	{
		"complex_compute",
		ComplexCompute::main,
	},
	{
		"draw_multi_indirect_buffer",
		DrawMultiIndirectBuffer::main
	}
};

int main(int argc, char** argv) {
	if(argc > 1) {
		for(int i=0; i<sizeof(entries)/sizeof(Entry); ++i) {
			if(strcmp(entries[i].name, argv[1]) == 0) {
				return entries[i].entry();
			}
		}
		printf("There is no sample with name: %s\n", argv[1]);
	}
	
	printf("Available samples:\n");
	for(int i=0; i<sizeof(entries)/sizeof(Entry); ++i) {
		printf("    %s\n", entries[i].name);
	}
	printf("\nUsage:\n");
	printf("%s SAMPLE_NAME\n", argv[0]);
	
	return 1;
}

