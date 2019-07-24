#include <vst.h>
#include <cstdio>

AudioEffect *createEffectInstance (audioMasterCallback audio_master) {
    return new NanceloidVST (audio_master);
}

NanceloidVST::NanceloidVST (audioMasterCallback audio_master)
    : AudioEffectX (audio_master, 0, PARAMETER_COUNT) {

    setNumInputs (0);
    setNumOutputs (2);
    setUniqueID ('nanc');
    canProcessReplacing ();
    isSynth ();

    // create the synth
    synth = new Nanceloid (new SawSource);
}

NanceloidVST::~NanceloidVST () {
    delete synth;
}

VstInt32 NanceloidVST::canDo (char *string) {
    if (!strcmp (string, "receiveVstEvents"))
        return 1;
    if (!strcmp (string, "receiveVstMidiEvent"))
        return 1;
    return -1;
}

VstInt32 NanceloidVST::getNumMidiInputChannels () {
    return 1;
}

VstInt32 NanceloidVST::getNumMidiOutputChannels () {
    return 0;
}

void NanceloidVST::setSampleRate (float rate) {
    AudioEffectX::setSampleRate (rate);

    // set the rate
    synth->set_rate (rate);
}

void NanceloidVST::processReplacing (float **inputs, float **outputs, VstInt32 frames) {
    while (frames--)
        *outputs[0]++ = *outputs[1]++ = synth->run ();
}

VstInt32 NanceloidVST::processEvents (VstEvents *event) {
    for (VstInt32 i = 0; i < event->numEvents; i++) {
        if ((event->events[i])->type == kVstMidiType) {
            VstMidiEvent *midi_event = (VstMidiEvent *) event->events[i];
            synth->midi ((uint8_t *) midi_event->midiData);
        }
    }
    return 1;
}


VstInt32 NanceloidVST::getVendorVersion () {
    return 1000; // idk
}

bool NanceloidVST::getEffectName (char *name) {
    vst_strncpy (name, "Nanceloid", kVstMaxEffectNameLen);
    return true;
}

bool NanceloidVST::getProductString (char *string) {
    vst_strncpy (string, "Nanceloid", kVstMaxProductStrLen);
    return true;
}

bool NanceloidVST::getVendorString (char *string) {
    vst_strncpy (string, "Negative Nancy", kVstMaxVendorStrLen);
    return true;
}

void NanceloidVST::setParameter (VstInt32 index, float value) {
    switch (index) {
        case PLACEHOLDER:
            // placeholder
            break;
    }
}

float NanceloidVST::getParameter (VstInt32 index, float value) {
    switch (index) {
        case PLACEHOLDER:
            return 0;
            break;
        default:
            return 0;
    }
}

void NanceloidVST::getParameterName (VstInt32 index, char *name) {
    vst_strncpy (name, parameter_names[index], kVstMaxParamStrLen);
}

void NanceloidVST::getParameterLabel (VstInt32 index, char *label) {
    vst_strncpy (label, parameter_labels[index], kVstMaxParamStrLen);
}

void NanceloidVST::getParameterDisplay (VstInt32 index, char *string) {
    switch (index) {
        case PLACEHOLDER:
            sprintf(string, "placeholder");
            break;
    }
}
