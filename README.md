# Negative Nancy's Waveguide Network Playground

a waveguide network engine with a planned vst frontend!

VERY wip lol

a standalone frontend wld b cool too

_TODO: add screenshots_

## Dependencies

In order to build the VST plugins you will need the following:
- `i686-w64-mingw32-g++`
- `x86_64-w64-mingw32-g++`
- the VST SDK 2.4

Obtain the VST SDK 2.4 and place the contents at the root of this repository in `vstsdk2.4`.
When placed correctly it should look like this:
```bash
$ ls vstsdk2.4/
artwork  bin  doc  index.html  pluginterfaces  public.sdk  vstgui.sf
```

## How to build

Run `make` to produce the 32-bit VST plugin `build/nnwgnp32.dll`, the 64-bit VST plugin `build/nnwgnp64.dll`, and the waveguide network engine test program `build/test`.

Run `make clean` to remove the `build` directory and its contents once it has been created.

## How to run

Load `build/nnwgnp32.dll` or `build/nnwgnp64.dll` into your DAW to use the VST plugin.

Run `make test` to run the waveguide network engine test program.

Run `make debug` to run the same thing in GDB.

## How to use

_TODO: tutorial, explanation, theory, etc_

## Files

The `src` directory contains the following:
- `test.c` contains the test program.
- `vst.h` and `vst.cpp` contain the VST plugin.
- `waveguide.h` and `waveguide.c` contain the waveguide network engine.
- `list.h` and `list.c` contain a linked list implementation.

After building, the `build` directory will contain the following:
- `test` a test program used for debugging the waveguide network engine.

## Documentation

tutorial and documentation: COMING EVENTUALLY!!

check `src/waveguide.h` and `src/waveguide.c` to see the waveguide simulation code

see `src/test.c` for a simple example of manually creating a simple chain of nodes with a source on one end and drain on the other

## Todo list

- basic vst
- basic vst interface
- de/serialization of waveguide network objects
- "exciter" nodes : generates sound by oscillating impedance in response to pressure
- "pump" nodes : provides steady stream of pressure values
- fix broken makefile lol
