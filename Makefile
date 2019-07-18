build/test: build/libnnwnp.a src/test.c
	gcc src/test.c -g -Iinclude -Lbuild -lnnwnp -lm -Wall -o build/test

build/libnnwnp.a: src/nnwnp.c include/nnwnp.h
	mkdir -p build
	gcc -g -c src/nnwnp.c -Iinclude -Wall -o build/nnwnp.o
	ar rcs build/libnnwnp.a build/nnwnp.o

# TODO: target for the vst plugin

.PHONY:
clean:
	rm -rf build

.PHONY:
run: build/test
	build/test

.PHONY:
debug: build/test
	gdb build/test
