#pragma once

#include <parameters.h>
#include <cstdint>

// represents a synth instance
class Nanceloid {
    private:
        struct {
            double note     = 0;    // midi note value
            double detune   = 0;    // offset in semitones
            double velocity = 0;    // note velocity (0 to 1)
            int start_time = 0;     // sample clock time of note event
        } note;                     // info about the current note to play

        // sampling parameters and timing
        double rate = 0;            // audio sampling rate
        double control_rate = 0;    // low frequency control rate
        int clock = 0;              // sample clock

        // state
        double sample = 0;          // last sample
        double osc_phase = 0;       // current phase of the primary oscillator
        double tremolo_phase = 0;   // current phase of tremolo lfo
        double vibrato_phase = 0;   // current phase of vibrato lfo
        double tremolo_osc = 0;     // output of tremolo lfo
        double vibrato_osc = 0;     // output of vibrato lfo
        double frequency = 0;       // current intended playing frequency
        double target_pressure = 0; // unfiltered current input pressure
        double pressure = 0;        // current input pressure

        // hardcoded parameters
        const double speed_of_sound = 34300;    // cm/s
        const int super_sampling = 2;
        const double pressure_smoothing = 100;
        const int control_rate_divider = 1000;  // sample clock divider for low frequency rate

    public:
        Nanceloid () {};
        ~Nanceloid () {};

        // update the sample rate
        void set_rate (double rate);

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
