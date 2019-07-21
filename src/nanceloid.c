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

    // musical and audio parameters
    p->vibrato_rate  = 4;
    p->vibrato_depth = 0.25;
    p->volume = 0.5;

}

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
    voice->osc = 0;
    voice->osc_phase = 0;
    voice->vibrato_phase = 0;

    return voice;
}

void destroy_voice (Voice *voice) {
    destroy_waveguide (voice->waveguide);
    free (voice);
}

void init_tract (Voice *voice) {

    // create a straight tube closed off at one end
    // (temporary)
    const int tube_length = 25;
    NN_Node *previous = NULL;
    for (int i = 0; i < tube_length; i++) {

        // create the next node
        NN_Node *node = spawn_node (voice->waveguide);

        // test
        //if (i == 10)
        //    set_admittance (node, 0.5);

        // link the node with the previous node
        // (unless this is the first node of course)
        if (previous != NULL)
            double_link_nodes (previous, node);
        else
            voice->source = terminate_node (node);

        // set the previous node for next iteration
        previous = node;

        // set ambient admittance
        if (i == tube_length - 1)
            set_admittance (node, AMBIENT_ADMITTANCE);
    }
}

// TODO: resize tract function

// return frequency in hertz given midi note
// equal temperament for now
double get_frequency (double note) {
    return 440.0 * pow (2.0, (note - 69) / 12);
}

// simple sawtooth source synth
double phonate_saw (Voice *voice) {

    // TODO: use lungs for intensity instead of velocity directly
    // and adjust lungs to note velocity

    // vibrato lfo
    double vibrato = sin (voice->vibrato_phase * 2 * M_PI) * voice->parameters.vibrato_depth;
    voice->vibrato_phase += voice->parameters.vibrato_rate / voice->rate;

    // saw osc
    double saw = voice->osc_phase;
    double freq = get_frequency (voice->note.note + voice->note.detune + vibrato);
    voice->osc_phase += freq / voice->rate;
    voice->osc_phase = fmod (voice->osc_phase, 1);

    // filter
    double sample = voice->osc;
    double delta = saw - sample;
    double k = pow (voice->note.velocity, 3);
    voice->osc += delta * k;

    return sample * voice->parameters.volume;
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

    // air pressure from lungs
    //add_energy (voice->source, voice->parameters.lungs);

    // vocal cord vibration
    double glottal_source = phonate (voice);
    add_energy (voice->source, glottal_source);

    // handle articulatory dynamics
    // TODO

    // set physical attributes
    // TODO

    // simulate acoustics
    run_waveguide (voice->waveguide);

    // return the drain output
    return collect_drain (voice->waveguide);
}
