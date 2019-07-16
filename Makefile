build/test: build/libnnwnp.a src/test.c
	gcc src/test.c -Iinclude -Lbuild -lnnwnp -Wall -o build/test

build/libnnwnp.a: src/nnwnp.c
	mkdir -p build
	gcc -c src/nnwnp.c -Iinclude -Wall -o build/nnwnp.o
	ar rcs build/libnnwnp.a build/nnwnp.o

# TODO: target for the vst plugin

.PHONY:
clean:
	rm -rf build

.PHONY:
run: build/test
	build/test
