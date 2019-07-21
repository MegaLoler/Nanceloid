#include <stdlib.h>
#include <math.h>
#include <nanceloid.h>

void init_parameters (Parameters *p) {

    // phonation parameters
    p->lungs            = 0; // not breathing
    p->glottal_tension  = 0; // modal voice
    p->laryngeal_height = 0; // neutral pitch

    // articulatory parameters
    p->lips_roundedness = 0;   // resting lips
    p->jaw_height       = 0.5; // resting jaw
    p->tongue_frontness = 0.5; // center tongue
    p->tongue_height    = 0;   // schwa position
    p->tongue_flatness  = 0;   // middle curvature

    // physical parameters
    p->acoustic_damping = DAMPING;
    p->physical_damping = 0.4;
    p->enunciation      = 0.75;
    p->frication        = TURBULENCE;
    p->surface_tension  = 0.5;
    p->tract_length     = 17.5;

    // musical parameters
    p->vibrato_rate  = 4;
    p->vibrato_depth = 0.25;

}

void init_tract (Voice *voice) {

    // create a straight tube closed off at one end
    // (temporary)
    const int tube_length = 20;
    NN_Node *previous = NULL;
    for (int i = 0; i < tube_length; i++) {

        // figure out what kind of node it should be
        NN_NodeType type = GUIDE;
        if (i == 0)
            type = SOURCE;
        else if (i == tube_length - 1)
            type = DRAIN;

        // create the next node
        NN_Node *node = spawn_node (voice->waveguide, type);

        // link the node with the previous node
        // (unless this is the first node of course)
        if (previous != NULL)
            link_nodes (previous, node);

        // set the previous node for next iteration
        previous = node;

        // set the active source and drain nodes
        if (i == 0)
            voice->source = node;
        else if (i == tube_length - 1)
            voice->drain = node;
    }

}

// TODO: resize tract function

Voice *create_voice (PhonationModel model, int rate) {
    Voice *voice = (Voice *) malloc (sizeof (Voice));

    init_parameters (&voice->parameters);
    voice->waveguide = create_waveguide ();
    init_tract (voice);
    voice->model = model;

    voice->note.note = 45;
    voice->note.detune = 0;
    voice->note.velocity = 1;

    voice->rate = rate;
    voice->saw_phase = 0;
    voice->vibrato_phase = 0;

    return voice;
}

void destroy_voice (Voice *voice) {
    destroy_waveguide (voice->waveguide);
    free (voice);
}

// return frequency in hertz given midi note
// equal temperament for now
double get_frequency (double note) {
    return 440.0 * pow (2.0, (note - 69) / 12);
}

// simple sawtooth source synth
double phonate_saw (Voice *voice) {

    // vibrato lfo
    double vibrato = sin (voice->vibrato_phase * 2 * M_PI) * voice->parameters.vibrato_depth;
    voice->vibrato_phase += voice->parameters.vibrato_rate / voice->rate;

    // saw osc
    double saw = voice->saw_phase * voice->note.velocity;
    double freq = get_frequency (voice->note.note + voice->note.detune + vibrato);
    voice->saw_phase += freq / voice->rate;
    voice->saw_phase = fmod (voice->saw_phase, 1);

    // TODO: low pass filter

    return saw;
}

// lf model of glottal excitation
double phonate_lf (Voice *voice) {
    // TODO
    return 0;
}

// produce glottal source sound sample
double phonate (Voice *voice) {
    switch (voice->model) {
        default:
        case SAWTOOTH:
            return phonate_saw (voice);
        case LF:
            return phonate_lf (voice);
    }
}

double step_voice (Voice *voice) {

    // handle phonation
    double glottal_source = phonate (voice);
    inject_energy (voice->waveguide, voice->source, glottal_source);

    // handle articulatory dynamics
    // TODO

    // set physical attributes
    // TODO

    // simulate acoustics
    run_waveguide (voice->waveguide);

    // return the drain output
    return net_node_energy (voice->drain);
}
