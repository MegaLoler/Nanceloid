#include <vst.h>
#include <cstdio>

AudioEffect *createEffectInstance (audioMasterCallback audio_master) {
    return new NN_VST (audio_master);
}

NN_VST::NN_VST (audioMasterCallback audio_master)
    : AudioEffectX (audio_master, 0, PARAMETER_COUNT) {

    setNumInputs (0);
    setNumOutputs (2);
    setUniqueID ('nanc');
    canProcessReplacing ();
    isSynth ();
}

NN_VST::~NN_VST () {
    destroy_voice (voice);
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

    // setup the voice synth
    voice = create_voice (SAWTOOTH, rate);

    // TODO: we need a set rate callback for the voice
    // TODO: in the mean time we should free an existing voice before creating a new one lol
}

void NN_VST::processReplacing (float **inputs, float **outputs, VstInt32 frames) {
    while (frames--)
        *outputs[0]++ = *outputs[1] = step_voice (voice);
}

VstInt32 NN_VST::processEvents (VstEvents *event) {
    for (VstInt32 i = 0; i < event->numEvents; i++) {
        if ((event->events[i])->type == kVstMidiType) {
            VstMidiEvent *midi_event = (VstMidiEvent *) event->events[i];
            process_midi (voice, (uint8_t *) midi_event->midiData);
        }
    }
    return 1;
}


VstInt32 NN_VST::getVendorVersion () {
    return 1000; // idk
}

bool NN_VST::getEffectName (char *name) {
    vst_strncpy (name, "Nanceloid", kVstMaxEffectNameLen);
    return true;
}

bool NN_VST::getProductString (char *string) {
    vst_strncpy (string, "Nanceloid", kVstMaxProductStrLen);
    return true;
}

bool NN_VST::getVendorString (char *string) {
    vst_strncpy (string, "Negative Nancy", kVstMaxVendorStrLen);
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
