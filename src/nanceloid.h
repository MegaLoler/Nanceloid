#pragma once

#include <parameters.h>
#include <cmath>
#include <cstdint>

// reperesents a vocal tract shape
class TractShape {
    private:
        double *diameter;   // let range be 0 to 1
        int length;

    public:
        TractShape (int length = 32) : length (length) {
            diameter = new double[length];
            for (int i = 0; i < length; i++)
                diameter[i] = 0.5;
        }

        ~TractShape () {
            delete diameter;
        }

        // get an interpolate value given a normalized position
        double sample (double i) {
            // linear interpolation ig lol
            double position = i * (length - 1);
            int i0 = (int) floor (position);
            int i1 = fmin (length - 1, i0 + 1);
            double weight1 = position - i0;
            double weight0 = 1 - weight1;
            double s0 = diameter[i0] * weight0;
            double s1 = diameter[i1] * weight1;
            return s0 + s1;
        }

        // the above but set instead
        void set_sample (double n, double sample) {
            // just set nearest ig lol
            int i = floor (n * length + 0.5);
            diameter[i] = sample;
        }

        // approach a given shape
        void crossfade (TractShape &target, double strength) {
            for (int i = 0; i < length; i++) {
                double position = (double) i / (length - 1);
                double target_sample = target.sample (position);
                diameter[i] += (target_sample - diameter[i]) * strength;
            }
        }
};

// represents a synth instance
class Nanceloid {
    private:
        // saved tract shapes for each midi patch number
        TractShape shapes[128];
        int shape_i = 0;

        // waveguide stuff
        int waveguide_length = 0;
        double reflection_damping = 0.04;
        // right and left going
        double *r = nullptr;
        double *l = nullptr;
        // backbuffers
        double *r_ = nullptr;
        double *l_ = nullptr;
        // reflection coefficients at each junction
        double *r_junction = nullptr;
        double *l_junction = nullptr;

        // current midi note
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
        const int super_sampling = 1;
        const double pressure_smoothing = 100;
        const int control_rate_divider = 1000;  // sample clock divider for low frequency rate

    public:
        Nanceloid () {};
        ~Nanceloid () ;

        // free resources
        void free ();

        // update the sample rate
        void set_rate (double rate);

        // create and initialize the waveguide
        void init ();

        // precalculate the reflection coefficients for each junction
        void update_reflections ();

        // run the voice for one frame setting stereo output samples
        void run (float *out);

        // runs at control rate
        void run_control ();

        // process a midi event
        void midi (uint8_t *data);

        // get the current shape
        TractShape &get_shape ();

        // public members
        Parameters params;      // the live synth parameters
        TractShape shape;       // the current instantaneous shape (not the preset)
};
