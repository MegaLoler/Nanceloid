# Nanceloid

A physically-modelled vocal synthesizer using digital waveguide networks!

*(currently reorganizing things quite a bit...)*

its a WIP O_O

_TODO: add screenshots_

## Dependencies

In order to build and run the standalone synth you will need the [JACK audio connection kit](http://jackaudio.org/).

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

Run `make` to produce the `build` directory containing the following:
- `nanceloid` is the standalone JACK client.
- `nanceloid32.dll` is the 32-bit version of the VST plugin.
- `nanceloid64.dll` is the 64-bit version of the VST plugin.
- `test` is a test program used for debugging the waveguide network engine.

Run `make clean` to remove the `build` directory and its contents after it has been created.

## How to run

Run `make run` to run the standalone synth.

Load `build/nanceloid32.dll` or `build/nanceloid64.dll` into your DAW to use the VST plugin.

Run `make test` to run the waveguide network engine test program.

Run `make debug` to run the same thing in GDB.

## How to use

_TODO: tutorial, explanation, theory, etc_

## Files

The `src` directory contains the following:
- `main.c` contains the the standalone JACK client.
- `vst.h` and `vst.cpp` contain the VST plugin.
- `midi.h` and `midi.c` contain the MIDI event handler.
- `nanceloid.h` and `nanceloid.c` contain the top-level synth.
- `waveguide.h` and `waveguide.c` contain the waveguide network engine.
- `list.h` and `list.c` contain a linked list implementation.
- `test.c` contains a test program.

## Documentation

tutorial and documentation: COMING EVENTUALLY!!

check `src/waveguide.h` and `src/waveguide.c` to see the waveguide simulation code

see `src/test.c` for a simple example of manually creating a simple chain of nodes with a source on one end and drain on the other

## Todo list

### Jack frontend stuff
- consider command line args
- switch to ALSA for midi

### VST stuff
- use nanceloid.h/c and midi.h/c
- basic vst interface

### Midi stuff
- normal note events
- preset phoneme mapped notes

### Nanceloid stuff
- all sorts of dynamics n stuff
- "exciter" nodes : generates sound by oscillating impedance in response to pressure
- lung "pump" node
- maybe a LF model glottis node....

### Waveguide stuff
- re dang write the whole thing

### Miscellaneous stuff
- fix broken makefile lol
- optimize ? ? idk lol
- consider other backends for the standalone (alsa? pulse? sdl? windows? rtmidi?)
