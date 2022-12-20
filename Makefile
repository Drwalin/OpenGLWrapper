
.PHONY: build
build:
	cmake -S . -B build
	cd build && make ${MAKEFLAGS}

.PHONY: clean
clean:
	rm -rf build

