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
        int rate = 0;                       // audio sampling rate
        int clock = 0;                      // sample clock
        int control_rate_divider = 1000;    // sample clock divider for low frequency rate

        // state
        float osc_phase = 0;        // current phase of the primary oscillator
        float tremolo_phase = 0;    // current phase of tremolo lfo
        float vibrato_phase = 0;    // current phase of vibrato lfo
        float tremolo_osc = 0;      // output of tremolo lfo
        float vibrato_osc = 0;      // output of vibrato lfo
        float frequency = 0;        // current intended playing frequency
        float pressure = 0;         // current input pressure

        // hardcoded parameters
        const float speed_of_sound = 34300; // cm/s

    public:
        Nanceloid () {};
        ~Nanceloid () {};

        // update the sample rate
        void set_rate (int rate);

        // create and initialize the waveguide
        void init ();

        // run the voice for one frame setting stereo output samples
        void run (float *out);

        // runs at control rate
        void run_control ();

        // process a midi event
        void midi (uint8_t *data);

        // public members
        Parameters params;      // the live synth parameters
};
