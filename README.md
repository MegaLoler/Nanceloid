# Nanceloid

A physically-modelled vocal synthesizer using digital waveguides!

its a work in progress O_O
most definitely not guaranteed to be in a usable/properly functioning state at any given time rn lol

i'm also quite in over my head and trying to learn so much stuff as i go!!! so i'm probably doing a lot of things wrong, lol, feel free to lemme kno if you are interested and/or know about this stuff and wanna help!!

_TODO: add screenshots (once there is a gui lol)_

## Dependencies

In order to build and run the standalone synth you will need the [libsoundio](http://libsound.io/) and [RtMidi](https://github.com/thestk/rtmidi).

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
- `nanceloid` is the standalone synth.
- `nanceloid32.dll` is the 32-bit version of the VST plugin.
- `nanceloid64.dll` is the 64-bit version of the VST plugin.
- `test` is a test program used for debugging the waveguide implementation.

Run `make clean` to remove the `build` directory and its contents after it has been created.

## How to run

Run `make run` to run the standalone synth.

Load `build/nanceloid32.dll` or `build/nanceloid64.dll` into your DAW to use the VST plugin.

Run `make test` to run the waveguide test program.

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
- `main.c` contains the the standalone synth.
- `vst.h` and `vst.cpp` contain the VST plugin.
- `nanceloid.h` and `nanceloid.c` contain the top-level synth.
- `glottal_source.h` contains an abstract class for providing a glottal source sound.
- `saw_source.h` and `saw_source.cpp` contain a simple saw wave based glottal source.
- `waveguide.h` and `waveguide.c` contain the waveguide implementation.
- `segment.h` and `segment.c` contain the waveguide segment class.
- `test.c` contains a test program.

## Immediate todo list

### Frontend stuff
- consider command line args
- simple gui

### VST stuff
- expose nanceloid parameters as vst parameters!!!
- basic gui

### Nanceloid stuff
- polyphony
- asdr envelope
- preset phoneme mapped notes
- standard midi controllers; bend etc
- note stack
- separation of some stuff into "control rate"
- nasal and lateral passages
- velic closure
- laryngeal extension
- dynamic larynx proportions
- lips extrusion
- jaw control
- surface tension
- copy energy from one waveguide to another function
- lf source model
- experimental source model

### Waveguide stuff
- more realistic turbulence
- "double buffer" instead of "flushing"

### Miscellaneous stuff
- fix broken makefile lol
- optimize ? ? idk lol
