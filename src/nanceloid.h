#pragma once

#include <waveguide.h>
#include <segment.h>
#include <glottal_source.h>

const double speed_of_sound = 34300; // cm/s
const double portamento = 0.001;
const double enunciation = 0.001;
const double neutral_impedance = 1;

// represents synth parameters
struct Parameters {

    // phonation parameters
    double lungs              = 0;    // continuous air pressure from the lungs (-1 to 1)
    double glottal_tension    = 0;    // how tightly shut the glottis is (-1 to 1)
    double laryngeal_height   = 0;    // vertical position of the larynx, extends tract (-1 to 1)

    // articulatory parameters
    double lips_roundedness   = 0;    // closedness of the lips (0 to 1)
    double jaw_height         = 0.5;  // jaw openness/height (0 to 1)
    double tongue_frontness   = 0.5;  // position of peak of tongue (0 to 1)
    double tongue_height      = 0;    // how close to touching the roof of the mouth (0 to 1)
    double tongue_flatness    = 0;    // distribution of the tongue curve (-1 to 1)
    double velic_closure      = 0;    // the closing off of the nasal cavity (0 to 1)

    // physical parameters
    double acoustic_damping   = 0.04; // sound absorbsion, loss of energy at reflections (0 to 1)
    double physical_damping   = 0.4;  // damping of tract reshaping (0 to 1)
    double enunciation        = 0.25; // strength of tract reshaping (0 to 1)
    double portamento         = 0.5;  // how quickly pitch and velocity change (0 to 1)
    double frication          = 0.1;  // turbulence coefficient (0 to 1)
    double surface_tension    = 0.5;  // tendency of constrictions to stick together (0 to 1)
    double tract_length       = 11;   // length of vocal tract (cm)
    double ambient_admittance = 10;   // admittance of the drain

    // musical and audio parameters
    double vibrato_rate       = 4;    // how quickly the singing vibrato should be (hz)
    double vibrato_depth      = 0.25; // how wide the vibrato peak should be (semitones)
    double frequency          = 0;    // vocal fold oscillation frequency
    double volume             = 0.5;  // overall volume (0 to 1)

};

// the note that you want to play
struct TargetNote {
    uint8_t note    = 57;// midi note value
    double detune   = 0; // offset in semitones
    double velocity = 1; // note velocity (0 to 1)
};

// represents a synth instance
class Voice {
    private:
        Waveguide *waveguide;       // the waveguide network to simulate the tract
        
        int larynx_start;           // number of nodes in the larynx
        int tongue_start;           // number of nodes in the tongue
        int lips_start;             // number of nodes in the lips

        Parameters parameters;      // the synth parameters
        GlottalSource source;       // how to model glottal source sound
        TargetNote note;            // what note to play

        int rate = 0;               // sample rate
        double osc = 0;             // last oscillator sample
        double osc_phase = 0;       // glottal oscillator current phase
        double vibrato_phase = 0;   // vibrato lfo current phase

        // return 12tet frequency given a midi note value
        static double get_frequency (double note);

        // move the admittance of a segment toward a given value
        void approach_admittance (Segment &segment, double target);

    public:
        Voice (GlottalSource source) : source (source) {}
        ~Voice ();

        // update the sample rate
        void set_rate (int rate);

        // run the voice for one frame and return a sample
        double run ();

        // create and initialize the waveguide
        void init ();

        // update the shape of the tract according to parameters
        void reshape ();

        // debug print the state of the parameters
        void debug ();

};
