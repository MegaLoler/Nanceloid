#ifndef NANCELOID_H
#define NANCELOID_H

#include <waveguide.h>

#define SPEED_OF_SOUND 34300 // cm/s
#define AMBIENT_ADMITTANCE 10
#define MAX 128
#define MAX_POLYPHONY 8
#define LARYNX_Z 5
#define NEUTRAL_Z 1
#define NASAL_Z 5
#define ENUNCIATION 0.001
#define PORTAMENTO 0.001



#ifdef __cplusplus
extern "C" {
#endif



// represents synth parameters
typedef struct Parameters {

    // phonation parameters
    double lungs;              // continuous air pressure from the lungs (-1 to 1)
    double glottal_tension;    // how tightly shut the glottis is (-1 to 1)
    double laryngeal_height;   // vertical position of the larynx, extends tract (-1 to 1)

    // articulatory parameters
    double lips_roundedness;   // closedness of the lips (0 to 1)
    double jaw_height;         // jaw openness/height (0 to 1)
    double tongue_frontness;   // position of peak of tongue (0 to 1)
    double tongue_height;      // how close to touching the roof of the mouth (0 to 1)
    double tongue_flatness;    // distribution of the tongue curve (-1 to 1)
    double velic_closure;      // the closing off of the nasal cavity (0 to 1)

    // physical parameters
    double acoustic_damping;   // sound absorbsion, loss of energy at reflections (0 to 1)
    double physical_damping;   // damping of tract reshaping (0 to 1)
    double enunciation;        // strength of tract reshaping (0 to 1)
    double portamento;         // how quickly pitch and velocity change (0 to 1)
    double frication;          // turbulence coefficient (0 to 1)
    double surface_tension;    // tendency of constrictions to stick together (0 to 1)
    double tract_length;       // length of vocal tract (cm)
    double ambient_admittance; // admittance of the drain

    // musical and audio parameters
    double vibrato_rate;       // how quickly the singing vibrato should be (hz)
    double vibrato_depth;      // how wide the vibrato peak should be (semitones)
    double frequency;          // vocal fold oscillation frequency
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
    
    NN_Node *larynx[MAX];       // nodes of the larynx
    NN_Node *tongue[MAX];       // nodes of the tongue
    NN_Node *lips[MAX];         // nodes of the lips
    NN_Node *nose[MAX];         // nodes of the nose
    int larynx_length;          // number of nodes in the larynx
    int tongue_length;          // number of nodes in the tongue
    int lips_length;            // number of nodes in the lips
    int nose_length;            // number of nodes in the nose

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

// update the shape of the tract according to parameters
void reshape_tract (Voice *voice);

// update the size of the tract by regenerated a new one
void resize_tract (Voice *voice);

// create and initialize a new synth instance
Voice *create_voice (PhonationModel model, int rate);

// destroy a synth instance
void destroy_voice (Voice *voice);

// run the simulation and return the next sample
double step_voice (Voice *voice);

// debug print the state of the parameters
void debug_parameters (Parameters parameters);



#ifdef __cplusplus
}
#endif



#endif
