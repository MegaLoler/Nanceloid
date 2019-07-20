# Negative Nancy's Waveguide Network Playground

a waveguide network engine with a planned vst frontend!

VERY wip lol

a standalone frontend wld b cool too

_TODO: add screenshots_

## Dependencies

_TODO: explanation of VST dependencies_

## How to build

Run `make` to produce the test program `build/test`.

## How to use

Run `make test` to run the test program.

## Files

The `src` directory contains the following:
- `test.c` contains the test program.
- `waveguide.h` and `waveguide.c` contain the waveguide network engine.
- `list.h` and `list.c` contain a linked list implementation.

After building, the `build` directory will contain the following:
- `test` a test program used for debugging.

## Documentation

tutorial and documentation: COMING EVENTUALLY!!

check `src/waveguide.h` and `src/waveguide.c` to see the waveguide simulation code

see `src/test.c` for a simple example of manually creating a simple chain of nodes with a source on one end and drain on the other

## Todo list

- fix broken makefile lol
- de/serialization of waveguide network objects
- "exciter" nodes : generates sound by oscillating impedance in response to pressure
- "pump" nodes : provides steady stream of pressure values
