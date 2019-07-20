# thanx S/O
TOP := $(dir $(lastword $(MAKEFILE_LIST)))

all: build/test

build/test: build/test.o build/waveguide.o build/list.o
	gcc -Wall -g -lm build/test.o build/waveguide.o build/list.o -o build/test

build/test.o: build src/test.c
	gcc -Wall -Isrc -g -c src/test.c -o build/test.o

build/waveguide.o: build src/waveguide.h src/waveguide.c
	gcc -Wall -Isrc -g -c src/waveguide.c -o build/waveguide.o

build/list.o: build src/list.h src/list.c
	gcc -Wall -Isrc -g -c src/list.c -o build/list.o

# TODO: target for the vst plugin

vstsdk2.4:
	$(error Please illegitimately obtain the VST SDK 2.4 and place the contents in "$(TOP)vstsdk2.4")

build:
	mkdir build

.PHONY:
clean:
	rm -rf build

.PHONY:
test: build/test
	build/test

.PHONY:
debug: build/test
	gdb build/test
