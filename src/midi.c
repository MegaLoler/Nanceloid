#include <midi.h>
#include <stdio.h>

// map a midi value to a given range
double map_to_range (uint8_t value, double min, double max) {
    return value / 127.0 * (max - min) + min;
}

void process_midi (Voice *voice, uint8_t *data) {

    // parse the data
    uint8_t type = data[0] & 0xf0;
    //uint8_t chan = data[0] & 0x0f;

    if (type == 0xb0) {

        // handle control events
        uint8_t id = data[1];
        uint8_t value = data[2];
        printf("Received midi controller event: 0x%x 0x%x\n", id, value);

        switch (id) {
            case CONTROLLER_GLOTTAL_TENSION:
                voice->parameters.glottal_tension = map_to_range (value, -1, 1);
                break;
            case CONTROLLER_LIPS_ROUNDEDNESS:
                voice->parameters.lips_roundedness = map_to_range (value, 0, 1);
                break;
            case CONTROLLER_JAW_HEIGHT:
                voice->parameters.jaw_height = map_to_range (value, 0, 1);
                break;
            case CONTROLLER_TONGUE_FRONTNESS:
                voice->parameters.tongue_frontness = map_to_range (value, 0, 1);
                break;
            case CONTROLLER_TONGUE_HEIGHT:
                voice->parameters.tongue_height = map_to_range (value, 0, 1);
                break;
            case CONTROLLER_TONGUE_FLATNESS:
                voice->parameters.tongue_flatness = map_to_range (value, -1, 1);
                break;
            case CONTROLLER_ENUNCIATION:
                voice->parameters.enunciation = map_to_range (value, 0, 1);
                break;
            case CONTROLLER_TRACT_LENGTH:
                voice->parameters.tract_length = map_to_range (value, 8, 24);
                break;
            // TODO: standard midi controllers such as modulation wheel and pitch bend
        }

        debug_parameters (voice->parameters);

    } else if (type == 0x80) {

        // handle note off events
        uint8_t note = data[1];
        uint8_t velocity = data[2];

        if (note == voice->note.note)
            voice->note.velocity = map_to_range (velocity, 0, 1);

    } else if (type == 0x90) {

        // handle note on events
        uint8_t note = data[1];
        uint8_t velocity = data[2];

        voice->note.note = note;
        voice->note.velocity = map_to_range (velocity, 0, 1);

    }

    // TODO: handle phoneme mapped notes
}
