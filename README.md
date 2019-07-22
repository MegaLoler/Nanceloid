# Nanceloid

A physically-modelled vocal synthesizer using digital waveguide networks!

its a work in progress O_O

_TODO: add screenshots (once there is a gui lol)_

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

### Synth parameters

| Parameter          | Midi Controller | Description                                 |
|--------------------|-----------------|---------------------------------------------|
| Lungs              | Note velocity   | Continuous air pressure from the lungs      |
| Glottis            | 0x15            | How tightly shut the vocal folds are        |
| Larynx             | Note pitch      | Vertical positioning of the larynx          |
| Lips               | 0x16            | Rounding and extrusion of the lips          |
| Jaw                | 0x17            | Elevation of the jaw                        |
| Frontness          | 0x18            | Tongue position                             |
| Height             | 0x19            | Tongue height                               |
| Flatness           | 0x1a            | Tongue spread                               |
| Velum              |                 | Velic closure                               |
| Acoustic damping   |                 | Sound absorbtion coefficient                |
| Physical damping   |                 | Damping of tract shape dynamics             |
| Enunciation        | 0x1b            | Force used to reshape tract                 |
| Portamento         |                 | How quickly pitch and velocity change       |
| Frication          |                 | Turbulence coefficient                      |
| Surface tension    |                 | "Stickiness"                                |
| Tract length       | 0x1c            | Overal length of the vocal tract            |
| Ambient admittance |                 | Acoustic ddmittance outside the vocal tract |
| Vibrato rate       |                 | Speed of vibrato                            |
| Vibrato depth      |                 | Intensity of vibrato                        |
| Frequency          |                 | Frequency of vocal fold oscillation         |
| Volume             |                 | Master volume                               |

_TODO: tutorial, explanation, theory, docs, etc_

## Files

The `src` directory contains the following:
- `main.c` contains the the standalone JACK client.
- `vst.h` and `vst.cpp` contain the VST plugin.
- `midi.h` and `midi.c` contain the MIDI event handler.
- `nanceloid.h` and `nanceloid.c` contain the top-level synth.
- `waveguide.h` and `waveguide.c` contain the waveguide network engine.
- `list.h` and `list.c` contain a linked list implementation.
- `test.c` contains a test program.

## Immediate todo list

### Frontend stuff
- consider command line args
- better alsa midi interfacing lol
- simple gui
- windows compatibility

### VST stuff
- expose nanceloid parameters as vst parameters!!!
- basic gui

### Midi stuff
- preset phoneme mapped notes

### Nanceloid stuff
- nasal and lateral passages
- velic closure
- laryngeal extension
- dynamic larynx proportions
- lips extrusion
- jaw control
- surface tension
- copy energy from one waveguide to another function
- polyphony saw
- lf source model
- experimental source model

### Waveguide stuff
- fix junction reflection...........
- fix destroying things dangit
- more realistic turbulence

### Miscellaneous stuff
- fix broken makefile lol
- optimize ? ? idk lol
- consider other backends for the standalone (alsa? pulse? sdl? windows? rtmidi?)
