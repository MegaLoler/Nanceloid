#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <nanceloid.h>

void debug_parameters (Parameters p) {
    printf ("[DEBUG PARAMETERS]\n");
    printf ("lungs              %.8f\n", p.lungs);
    printf ("glottal_tension    %.8f\n", p.glottal_tension);
    printf ("laryngeal_height   %.8f\n", p.laryngeal_height);
    printf ("lips_roundedness   %.8f\n", p.lips_roundedness);
    printf ("jaw_height         %.8f\n", p.jaw_height);
    printf ("tongue_frontness   %.8f\n", p.tongue_frontness);
    printf ("tongue_height      %.8f\n", p.tongue_height);
    printf ("tongue_flatness    %.8f\n", p.tongue_flatness);
    printf ("velic_closure      %.8f\n", p.velic_closure);
    printf ("acoustic_damping   %.8f\n", p.acoustic_damping);
    printf ("physical_damping   %.8f\n", p.physical_damping);
    printf ("enunciation        %.8f\n", p.enunciation);
    printf ("frication          %.8f\n", p.frication);
    printf ("surface_tension    %.8f\n", p.surface_tension);
    printf ("tract_length       %.8f\n", p.tract_length);
    printf ("ambient_admittance %.8f\n", p.ambient_admittance);
    printf ("vibrato_rate       %.8f\n", p.vibrato_rate);
    printf ("vibrato_depth      %.8f\n", p.vibrato_depth);
    printf ("volume             %.8f\n", p.volume);
    printf ("\n");
}

void init_parameters (Parameters *p) {

    // phonation parameters
    p->lungs            = 0; // not breathing
    p->glottal_tension  = 0; // modal voice
    p->laryngeal_height = 0; // neutral pitch

    // articulatory parameters
    p->lips_roundedness   = 0;   // resting lips
    p->jaw_height         = 0.5; // resting jaw
    p->tongue_frontness   = 0.5; // center tongue
    p->tongue_height      = 0;   // schwa position
    p->tongue_flatness    = 0;   // middle curvature
    p->velic_closure      = 1;   // no nasal

    // physical parameters
    p->acoustic_damping   = DAMPING;
    p->physical_damping   = 0.4;
    p->enunciation        = 0.75;
    p->frication          = TURBULENCE;
    p->surface_tension    = 0.5;
    p->tract_length       = 17.5;
    p->ambient_admittance = AMBIENT_ADMITTANCE;

    // musical and audio parameters
    p->vibrato_rate  = 4;
    p->vibrato_depth = 0.25;
    p->volume        = 0.5;

}

Voice *create_voice (PhonationModel model, int rate) {
    Voice *voice = (Voice *) malloc (sizeof (Voice));
    voice->model = model;

    voice->note.note = 45;
    voice->note.detune = 0;
    voice->note.velocity = 1;

    voice->rate = rate;
    voice->osc = 0;
    voice->osc_phase = 0;
    voice->vibrato_phase = 0;

    init_parameters (&voice->parameters);
    init_tract (voice);
    reshape_tract (voice);

    return voice;
}

void destroy_voice (Voice *voice) {
    destroy_waveguide (voice->waveguide);
    free (voice);
}

void reshape_tract (Voice *voice) {

    // shape the lips
    for (int i = 0; i < voice->lips_length; i++) {
        NN_Node *node = voice->lips[i];
        set_admittance (node, 1 / NEUTRAL_Z * (1 - voice->parameters.lips_roundedness));
    }

    // shape the tongue
    for (int i = 0; i < voice->tongue_length; i++) {
        NN_Node *node = voice->tongue[i];

        // TEMP
        double unit_pos = i / (double)(voice->tongue_length - 1);
        double phase = unit_pos - voice->parameters.tongue_frontness;
        double value = cos(phase * M_PI / 2) * voice->parameters.tongue_height;
        double unit_area = 1 - value;
        set_admittance (node, 1 / NEUTRAL_Z * unit_area);
    }

}

void init_tract (Voice *voice) {

    if (voice->waveguide != NULL)
        destroy_waveguide (voice->waveguide);
    voice->waveguide = create_waveguide ();

    // calculate the length of a single node
    double unit = (double) SPEED_OF_SOUND / voice->rate;

    // calculate the positions of various landmarks along the tract
    double larynx_start = voice->parameters.tract_length * 0;
    double tongue_start = voice->parameters.tract_length * 0.2;
    double lips_start   = voice->parameters.tract_length * 0.9;

    voice->larynx_length = 0;
    voice->tongue_length = 0;
    voice->lips_length = 0;

    printf ("%f\n", unit);

    // generate enough nodes to meet the desired length
    // plus one extra for the drain
    int i = 0;
    NN_Node *previous = NULL;
    double required_length = voice->parameters.tract_length + unit;
    while (unit * i < required_length) {

        // create the next node
        NN_Node *node = spawn_node (voice->waveguide);

        // figure out where we are in the tract
        double amount = unit * i;

        // set landmark nodes
        if (amount >= lips_start) {
            voice->lips[voice->lips_length++] = node;
            set_impedance (node, NEUTRAL_Z);

        } else if (amount >= tongue_start) {
            voice->tongue[voice->tongue_length++] = node;
            set_impedance (node, NEUTRAL_Z);

        } else if (amount >= larynx_start) {
            voice->larynx[voice->larynx_length++] = node;
            set_impedance (node, LARYNX_Z);

        }

        // link the node with the previous node
        // (unless this is the first node of course)
        if (previous != NULL)
            double_link_nodes (previous, node);
        else
            voice->source = terminate_node (node);

        // set the previous node for next iteration
        previous = node;

        // next
        i++;
    }

    // the last node is the drain and not part of the lips
    voice->lips_length--;

    // set ambient admittance
    set_admittance (previous, voice->parameters.ambient_admittance);

}

// TODO: resize tract function

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
    double saw = voice->osc_phase;
    double freq = get_frequency (voice->note.note + voice->note.detune + vibrato);
    voice->osc_phase += freq / voice->rate;
    voice->osc_phase = fmod (voice->osc_phase, 1);

    // intensity depending on glottal tension
    double intensity = voice->parameters.lungs * (voice->parameters.glottal_tension + 1) / 2;
    double loudness = voice->parameters.lungs * (fmin (voice->parameters.glottal_tension, 0) + 1);

    // filter
    double sample = voice->osc;
    double delta = saw - sample;
    double k = pow (intensity, 3);
    voice->osc += delta * k;

    return sample * loudness;
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
    double opening = fmax (-voice->parameters.glottal_tension, 0);
    add_energy (voice->source, voice->parameters.lungs * opening);

    // vocal cord vibration
    double glottal_source = phonate (voice);
    add_energy (voice->source, glottal_source);

    // handle articulatory dynamics
    // TODO
    reshape_tract (voice);

    // set physical attributes
    voice->parameters.lungs = voice->note.velocity;

    // simulate acoustics
    run_waveguide (voice->waveguide);

    // return the drain output
    return collect_drain (voice->waveguide) * voice->parameters.volume;
}
