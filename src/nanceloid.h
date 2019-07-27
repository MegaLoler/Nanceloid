#pragma once

#include <cstdint>
#include <waveguide.h>
#include <segment.h>
#include <glottal_source.h>

class GlottalSource;

// midi controller mappings
const int controller_vibrato          = 0x01;
const int controller_glottal_tension  = 0x15;
const int controller_lips_roundedness = 0x16;
const int controller_jaw_height       = 0x17;
const int controller_tongue_frontness = 0x18;
const int controller_tongue_height    = 0x19;
const int controller_velum            = 0x1a;
const int controller_enunciation      = 0x1b;
const int controller_tract_length     = 0x1c;

// multipliers, mostly
const double speed_of_sound = 34300; // cm/s
const double portamento = 0.001;
const double enunciation = 0.001;
const double neutral_impedance = 1;

// represents synth parameters
struct Parameters {

    // phonation parameters
    double lungs              = 0;    // continuous air pressure from the lungs (-1 to 1)
    double glottal_tension    = 0.5;  // how tightly shut the glottis is (-1 to 1)
    double laryngeal_height   = 0;    // vertical position of the larynx, extends tract (-1 to 1)

    // articulatory parameters
    double lips_roundedness   = 0;    // closedness of the lips (0 to 1)
    double jaw_height         = 0.5;  // jaw openness/height (0 to 1)
    double tongue_frontness   = 0;    // position of peak of tongue (0 to 1)
    double tongue_height      = 0;    // how close to touching the roof of the mouth (0 to 1)
    double tongue_flatness    = 0;    // distribution of the tongue curve (-1 to 1)
    double velic_closure      = 1;    // the closing off of the nasal cavity (0 to 1)

    // physical parameters
    double acoustic_damping   = 0.04; // sound absorbsion, loss of energy at reflections (0 to 1)
    double enunciation        = 0.25; // strength of tract reshaping (0 to 1)
    double portamento         = 0.65; // how quickly pitch and velocity change (0 to 1)
    double frication          = 0.1;  // turbulence coefficient (0 to 1)
    double surface_tension    = 0.5;  // tendency of constrictions to stick together (0 to 1)
    double tract_length       = 24;   // length of vocal tract (cm)
    double ambient_admittance = 10;   // admittance of the drain

    // musical and audio parameters
    double vibrato_rate       = 4;    // how quickly the singing vibrato should be (hz)
    double vibrato_depth      = 0.25; // how wide the vibrato peak should be (semitones)
    double velocity           = 0.5;  // how much incoming note velocity has any effect (0 to 1)
    double panning            = 0;    // left right panning (-1 to 1)
    double volume             = 0.5;  // overall volume (0 to 1)

};

// the note that you want to play
struct TargetNote {
    uint8_t note    = 40;// midi note value
    double detune   = 0; // offset in semitones
    double velocity = 0; // note velocity (0 to 1)
};

// represents a synth instance
class Nanceloid {
    private:
        Waveguide *throat;          // the waveguide to simulate the throat
        Waveguide *mouth;           // the waveguide to simulate the mouth
        Waveguide *nose;            // the waveguide to simulate the nose
        
        int length;                 // total length
        int larynx_start;           // start segment number of the larynx
        int tongue_start;           // start segment number of the tongue
        int nose_start;             // start segment number of the nasal junction
        int lips_start;             // start segment number of the lips

        GlottalSource *source;      // how to model glottal source sound
        Parameters parameters;      // the synth parameters
        TargetNote note;            // what note to play

        int rate = 0;               // sample rate

        // map a midi value to a given range
        static double map_to_range (uint8_t value, double min, double max);

        // map incoming note velocity to appropriate normalized value
        double map_velocity (uint8_t value);

        // move the admittance of a segment toward a given value
        void approach_admittance (Segment &segment, double target, double coefficient);

        // get a segment along the tract of the throat and mouth
        Segment &get_throat_mouth_segment (int i);

    public:
        Nanceloid (GlottalSource *source) : source (source) {}
        ~Nanceloid ();

        // update the sample rate
        void set_rate (int rate);

        // run the voice for one frame and return a sample
        double run ();

        // create and initialize the waveguide
        void init ();

        // update the shape of the tract according to parameters
        // if set is true it will instantly set the shape rather thna approach
        void reshape (bool set = false);

        // debug print the state of the parameters
        void debug ();

        // process a midi event
        void midi (uint8_t *data);

        // get the the parameters
        Parameters &get_parameters ();

        friend class GlottalSource;
        friend class SawSource;
        friend class NanceloidVST;
};
