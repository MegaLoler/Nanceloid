#include <stdlib.h>
#include <nanceloid.h>

void init_parameters (Parameters *p) {

    // phonation parameters
    p->lungs            = 0; // not breathing
    p->glottal_tension  = 0; // modal voice
    p->laryngeal_height = 0; // neutral pitch

    // articulatory parameters
    p->lips_extrusion   = 0;   // resting lips
    p->lips_roundedness = 0.5; // neutral position
    p->jaw_height       = 0.5; // neutral position
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

}

void init_tract (NN_Waveguide *waveguide) {

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
        NN_Node *node = spawn_node (waveguide, type);

        // link the node with the previous node
        // (unless this is the first node of course)
        if (previous != NULL)
            link_nodes (previous, node);

        // set the previous node for next iteration
        previous = node;

        // set the active source and drain nodes
        //if (i == 0)
        //    source_node = node;
        //else if (i == tube_length - 1)
        //    drain_node = node;
    }

}

// TODO: resize tract function

Voice *create_voice () {
    Voice *voice = (Voice *) malloc (sizeof (Voice));
    init_parameters (&voice->parameters);
    voice->waveguide = create_waveguide ();
    init_tract (voice->waveguide);
    return voice;
}

void destroy_voice (Voice *voice) {
    destroy_waveguide (voice->waveguide);
    free (voice);
}

double step_voice (Voice *voice) {

    // handle articulatory dynamics
    // TODO

    // set physical attributes
    // TODO

    // simulate acoustics
    run_waveguide (voice->waveguide);

    // return the drain output
    // TODO
    return 0;
}
