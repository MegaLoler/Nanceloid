#include <waveguide.h>

// represents synth parameters
typedef struct Parameters {

    // phonation parameters
    double lungs;            // continuous air pressure from the lungs (-1 to 1)
    double glottal_tension;  // how tightly shut the glottis is (0 to 1)
    double laryngeal_height; // effects pitch and extents tract (0 to 1)

    // articulatory parameters
    double lips_extrusion;   // effects length and area in front of mouth (0 to 1)
    double lips_roundedness; // closedness of the lips (0 to 1)
    double jaw_height;       // jaw openness/height (0 to 1)
    double tongue_frontness; // position of peak of tongue (0 to 1)
    double tongue_height;    // how close to touching the roof of the mouth (0 to 1)
    double tongue_flatness;  // distribution of the tongue curve (0 to 1)

    // physical parameters
    double acoustic_damping; // sound absorbsion, loss of energy at reflections (0 to 1)
    double physical_damping; // damping of tract reshaping (0 to 1)
    double enunciation;      // strength of tract reshaping (0 to 1)
    double frication;        // turbulence coefficient (0 to 1)
    double surface_tension;  // tendency of constrictions to stick together (0 to 1)
    double tract_length;     // length of vocal tract (cm)

} Parameters;

// represents a synth instance
typedef struct Voice {

    NN_Waveguide *waveguide;
    Parameters parameters;

} Voice;

// initialize a parameters struct
void init_parameters (Parameters *p);

// initialize the waveguide
void init_tract (NN_Waveguide *waveguide);

// create and initialize a new synth instance
Voice *create_voice ();

// destroy a synth instance
void destroy_voice (Voice *voice);

// run the simulation and return the next sample
double step_voice (Voice *voice);
