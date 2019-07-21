#include <vst.h>
#include <cstdio>

AudioEffect *createEffectInstance (audioMasterCallback audio_master) {
    return new NN_VST (audio_master);
}

void NN_VST::init_waveguide () {
    // delete an existing one if it has already been established
    destroy_waveguide (waveguide);

    // create a new one
    waveguide = create_waveguide ();

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
        if (i == 0)
            source_node = node;
        else if (i == tube_length - 1)
            drain_node = node;
    }
}

NN_VST::NN_VST (audioMasterCallback audio_master)
    : AudioEffectX (audio_master, 0, PARAMETER_COUNT) {

    // TODO: dynamically set num ins and outs depending on how many source and drain nodes there are
    setNumInputs (1);
    setNumOutputs (1);
    setUniqueID ('nanc');
    canProcessReplacing ();
    isSynth ();

    // setup the waveguide network
    init_waveguide ();
}

NN_VST::~NN_VST () {
    destroy_waveguide (waveguide);
}

VstInt32 NN_VST::canDo (char *string) {
    if (!strcmp (string, "receiveVstEvents"))
        return 1;
    if (!strcmp (string, "receiveVstMidiEvent"))
        return 1;
    return -1;
}

VstInt32 NN_VST::getNumMidiInputChannels () {
    return 1;
}

VstInt32 NN_VST::getNumMidiOutputChannels () {
    return 0;
}

void NN_VST::setSampleRate (float rate) {
    AudioEffectX::setSampleRate (rate);
}

void NN_VST::processReplacing (float **inputs, float **outputs, VstInt32 frames) {
    float *in = inputs[0];
    float *out = outputs[0];

    while (frames--) {
        inject_energy (waveguide, source_node, *in++);
        run_waveguide (waveguide);
        *out++ = net_node_energy (drain_node);
    }
}

VstInt32 NN_VST::processEvents (VstEvents *event) {
    for (VstInt32 i = 0; i < event->numEvents; i++) {
        if ((event->events[i])->type != kVstMidiType)
            continue;

        /*
        VstMidiEvent *midi_event = (VstMidiEvent *) event->events[i];
        uint8_t *data = event->midiData;
        VstInt32 type = data[0] & 0xf0;
        VstInt32 chan = data[0] & 0x0f;

        if(type == 0xb0) {
            // control signal
            VstInt32 id = data[1];
            VstInt32 value = data[2];

        } else if(type == 0x80) {
            // note off
            VstInt32 note = data[1];
            VstInt32 velocity = data[2];

        } else if(type == 0x90) {
            // note on
            VstInt32 note = data[1];
            VstInt32 velocity = data[2];

        }
        */
    }
    return 1;
}


VstInt32 NN_VST::getVendorVersion () {
    return 1000; // idk
}

bool NN_VST::getEffectName (char *name) {
    vst_strncpy (name, "Negative Nancy's Waveguide Network Playground", kVstMaxEffectNameLen);
    return true;
}

bool NN_VST::getProductString (char *string) {
    vst_strncpy (string, "NNWGNP", kVstMaxProductStrLen);
    return true;
}

bool NN_VST::getVendorString (char *string) {
    vst_strncpy (string, "MegaLoler / Aardbei / Negative Nancy", kVstMaxVendorStrLen);
    return true;
}

void NN_VST::setParameter (VstInt32 index, float value) {
    switch (index) {
        case PLACEHOLDER:
            // placeholder
            break;
    }
}

float NN_VST::getParameter (VstInt32 index, float value) {
    switch (index) {
        case PLACEHOLDER:
            return 0;
            break;
        default:
            return 0;
    }
}

void NN_VST::getParameterName (VstInt32 index, char *name) {
    vst_strncpy (name, parameter_names[index], kVstMaxParamStrLen);
}

void NN_VST::getParameterLabel (VstInt32 index, char *label) {
    vst_strncpy (label, parameter_labels[index], kVstMaxParamStrLen);
}

void NN_VST::getParameterDisplay (VstInt32 index, char *string) {
    switch (index) {
        case PLACEHOLDER:
            sprintf(string, "placeholder");
            break;
    }
}
