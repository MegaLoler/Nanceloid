#ifndef NANCELOID_H
#define NANCELOID_H

#include <waveguide.h>

#define AMBIENT_ADMITTANCE 10 //INFINITY

// represents synth parameters
typedef struct Parameters {

    // phonation parameters
    double lungs;              // continuous air pressure from the lungs (-1 to 1)
    double glottal_tension;    // how tightly shut the glottis is (-1 to 1)
    double laryngeal_height;   // effects pitch and extents tract (-1 to 1)

    // articulatory parameters
    double lips_roundedness;   // closedness of the lips (0 to 1)
    double jaw_height;         // jaw openness/height (0 to 1)
    double tongue_frontness;   // position of peak of tongue (0 to 1)
    double tongue_height;      // how close to touching the roof of the mouth (0 to 1)
    double tongue_flatness;    // distribution of the tongue curve (-1 to 1)

    // physical parameters
    double acoustic_damping;   // sound absorbsion, loss of energy at reflections (0 to 1)
    double physical_damping;   // damping of tract reshaping (0 to 1)
    double enunciation;        // strength of tract reshaping (0 to 1)
    double frication;          // turbulence coefficient (0 to 1)
    double surface_tension;    // tendency of constrictions to stick together (0 to 1)
    double tract_length;       // length of vocal tract (cm)
    double ambient_admittance; // admittance of the drain

    // musical and audio parameters
    double vibrato_rate;       // how quickly the singing vibrato should be (hz)
    double vibrato_depth;      // how wide the vibrato peak should be (semitones)
    double volume;             // overall volume (0 to 1)

} Parameters;

// what model to use to generate glottal source
typedef enum PhonationModel {
    SAWTOOTH,
    LF
} PhonationModel;

// the note that you want to play
typedef struct TargetNote {
    uint8_t note;    // midi note value
    double detune;   // offset in semitones
    double velocity; // note velocity (0 to 1)
} TargetNote;

// represents a synth instance
typedef struct Voice {

    NN_Waveguide *waveguide;    // the waveguide network to simulate the tract
    NN_Link *source;            // the link to inject glottal source sound

    Parameters parameters;      // the synth parameters
    PhonationModel model;       // how to model glottal source sound
    TargetNote note;            // what note to play

    int rate;                   // sample rate
    double osc;                 // last oscillator sample
    double osc_phase;           // glottal oscillator current phase
    double vibrato_phase;       // vibrato lfo current phase

} Voice;

// initialize a parameters struct
void init_parameters (Parameters *p);

// initialize the waveguide of a synth
void init_tract (Voice *voice);

// create and initialize a new synth instance
Voice *create_voice ();

// destroy a synth instance
void destroy_voice (Voice *voice);

// run the simulation and return the next sample
double step_voice (Voice *voice);

// debug print the state of the parameters
void debug_parameters (Parameters parameters);

#endif
