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
            velic_closure += (target.velic_closure - velic_closure) * strength;
        }

        // public members
        double velic_closure = 1;       // closure of the nasal cavity opening
};

// represents a synth instance
class Nanceloid {
    private:
        // saved tract shapes for each midi patch number
        TractShape shapes[128];
        int shape_i = 0;

        // waveguide stuff
        int waveguide_length = 0;
        int nose_length = 0;
        int throat_i = 0;
        int mouth_i = 0;
        double reflection_damping = 0.01;
        double max_impedance = 500;
        double epsilon = 0.00001;
        // right and left going
        double *r = nullptr;
        double *l = nullptr;
        // backbuffers
        double *r_ = nullptr;
        double *l_ = nullptr;
        // reflection coefficients at each junction
        double *r_junction = nullptr;
        double *l_junction = nullptr;
        // right and left going for the nose
        double *nr = nullptr;
        double *nl = nullptr;
        // backbuffers for the nose
        double *nr_ = nullptr;
        double *nl_ = nullptr;
        // nose throat mouth junction stuff
        double throat_refl_c;
        double mouth_refl_c;
        double nose_refl_c;
        double throat_to_mouth_w;
        double throat_to_nose_w;
        double mouth_to_throat_w;
        double mouth_to_nose_w;
        double nose_to_throat_w;
        double nose_to_mouth_w;

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
        double dt;                  // sampling rate delta time

        // state
        double sample = 0;          // last sample
        double tremolo_phase = 0;   // current phase of tremolo lfo
        double vibrato_phase = 0;   // current phase of vibrato lfo
        double tremolo_osc = 0;     // output of tremolo lfo
        double vibrato_osc = 0;     // output of vibrato lfo
        double frequency = 0;       // current intended playing frequency
        double target_pressure = 0; // unfiltered current input pressure
        double pressure = 0;        // subglottal pressure from lungs
        double voicing = 0;         // related to glottal area at rest
        double cord_tension = 0;    // tension of the vocal folds
        double x = 0;
        double v = 0;

        // hardcoded parameters
        const double speed_of_sound = 34300;    // cm/s
        const int super_sampling = 1;
        const double pressure_smoothing = 100;
        const int control_rate_divider = 1000;  // sample clock divider for low frequency rate

        // free resources
        void free ();

        // create and initialize the waveguide
        void init ();

        // get the impedance given the index of the waveguide segment
        double get_impedance (int i);

        // precalculate the reflection coefficients for each junction
        void update_reflections ();

        // runs at control rate
        void run_control ();

    public:
        Nanceloid () {};
        ~Nanceloid () ;

        // update the sample rate
        void set_rate (double rate);

        // run the voice for one frame setting stereo output samples
        void run (float *out);

        // process a midi event
        void midi (uint8_t *data);

        // get the current shape
        TractShape &get_shape ();

        // get the id of the current shape
        int get_shape_id ();

        // set the current shape given its id
        void set_shape_id (int id);

        // play a note
        void note_on (int note, double velocity);

        // stop playing
        void note_off (int note);

        // returns the currently playing note
        // -1 for not playing
        int playing_note ();

        // get the current intended playing frequency
        double get_frequency ();

        // get the current voicing
        double get_voicing ();

        // public members
        Parameters params;      // the live synth parameters
        TractShape shape;       // the current instantaneous shape (not the preset)
};
