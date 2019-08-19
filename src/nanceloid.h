#pragma once

#include <parameters.h>
#include <cstdint>

// represents a synth instance
class Nanceloid {
    private:
        struct {
            float note     = 40;    // midi note value
            float detune   = 0;     // offset in semitones
            float velocity = 0;     // note velocity (0 to 1)
            int start_time = 0;     // sample clock time of note event
        } note;                     // info about the current note to play

        // sampling parameters and timing
        int clock = 0;                      // sample clock
        int sample_rate = 0;                // audio sampling rate
        int control_rate_divider = 1000;    // sample clock divider for low frequency rate

        // modulation state and playing target frequency
        float tremolo_phase = 0;
        float vibrato_phase = 0;
        float frequency = 0;

        // hardcoded parameters
        const float speed_of_sound = 34300; // cm/s

    public:
        Nanceloid () {};
        ~Nanceloid () {};

        // update the sample rate
        void set_sample_rate (int rate);

        // create and initialize the waveguide
        void init ();

        // run the voice for one frame and return a stereo sample
        float *run ();

        // runs at control rate
        void run_control ();

        // process a midi event
        void midi (uint8_t *data);

        // public members
        Parameters parameters;      // the live synth parameters
};
