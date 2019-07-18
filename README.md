# Negative Nancy's Waveguide Network Playground

a waveguide network engine with a planned vst frontend!

VERY wip lol

a standalone frontend wld b cool too

## How to build

just type `make` and it'll build the library and a test program in the `build` directory (which it will create)

`make run` will run the test if u want

## Files

`include/nnwnp.h` is the C header for using the library

`build/libnnwnp.a` is the library itself

## Documentation

tutorial and documentation: COMING EVENTUALLY!!

check `include/nnwnp.h` for explanation of the api ;o

see `src/test.c` for a simple example of manually creating a simple chain of nodes with a source on one end and drain on the other

## TODO

- proper array lists with remove arbitrary element function
- deleting links from nodes, and nodes from waveguide network objects
- de/serialization of waveguide network objects
- "exciter" nodes : generates sound by oscillating impedance in response to pressure
- "pump" nodes : provides steady stream of pressure values
