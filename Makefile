build/test: build/libnnwnp.a src/test.c
	gcc -g src/test.c -Iinclude -Lbuild -lnnwnp -lm -Wall -o build/test

build/libnnwnp.a: src/nnwnp.c src/list.c include/*.h
	mkdir -p build
	gcc -g -c src/nnwnp.c -Iinclude -Wall -o build/nnwnp.o
	gcc -g -c src/list.c -Iinclude -Wall -o build/list.o
	ar rcs build/libnnwnp.a build/nnwnp.o build/list.o

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
