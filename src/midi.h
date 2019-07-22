#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>
#include <nanceloid.h>

// midi controller parameter mappings
#define CONTROLLER_GLOTTAL_TENSION  0x15
#define CONTROLLER_LIPS_ROUNDEDNESS 0x16
#define CONTROLLER_JAW_HEIGHT       0x17
#define CONTROLLER_TONGUE_FRONTNESS 0x18
#define CONTROLLER_TONGUE_HEIGHT    0x19
#define CONTROLLER_TONGUE_FLATNESS  0x1a
#define CONTROLLER_ENUNCIATION      0x1b
#define CONTROLLER_TRACT_LENGTH     0x1c




#ifdef __cplusplus
extern "C" {
#endif



// process a midi event
void process_midi (Voice *voice, uint8_t *data);



#ifdef __cplusplus
}
#endif



#endif
