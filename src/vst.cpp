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
        case LUNGS:
            synth->parameters.lungs = map_to_range (value, -1, 1);
            break;
        case GLOTTIS:
            synth->parameters.glottal_tension = map_to_range (value, -1, 1);
            break;
        case LARYNX:
            synth->parameters.laryngeal_height = map_to_range (value, -1, 1);
            break;
        case LIPS:
            synth->parameters.lips_roundedness = map_to_range (value, 0, 1);
            break;
        case JAW:
            synth->parameters.jaw_height = map_to_range (value, 0, 1);
            break;
        case FRONTNESS:
            synth->parameters.tongue_frontness = map_to_range (value, 0, 1);
            break;
        case HEIGHT:
            synth->parameters.tongue_height = map_to_range (value, 0, 1);
            break;
        case FLATNESS:
            synth->parameters.tongue_flatness = map_to_range (value, -1, 1);
            break;
        case VELUM:
            synth->parameters.velic_closure = map_to_range (value, 0, 1);
            break;
        case ACOUSTIC_DAMPING:
            synth->parameters.acoustic_damping = map_to_range (value, 0, 1);
            break;
        case ENUNCIATION:
            synth->parameters.enunciation = map_to_range (value, 0, 1);
            break;
        case PORTAMENTO:
            synth->parameters.portamento = map_to_range (value, 0, 1);
            break;
        case FRICATION:
            synth->parameters.frication = map_to_range (value, 0, 1);
            break;
        case SURFACE_TENSION:
            synth->parameters.surface_tension = map_to_range (value, 0, 1);
            break;
        case TRACT_LENGTH:
            synth->parameters.tract_length = map_to_range (value, 8, 24);
            break;
        case AMBIENT_ADMITTANCE:
            synth->parameters.ambient_admittance = map_to_range (value, 0, 100);
            break;
        case VIBRATO_RATE:
            synth->parameters.vibrato_rate = map_to_range (value, 0, 16);
            break;
        case VIBRATO_DEPTH:
            synth->parameters.vibrato_depth = map_to_range (value, 0, 2);
            break;
        case VELOCITY_WEIGHT:
            synth->parameters.velocity = map_to_range (value, 0, 1);
            break;
        case PANNING:
            synth->parameters.panning = map_to_range (value, -1, 1);
            break;
        case VOLUME:
            synth->parameters.volume = map_to_range (value, 0, 1);
            break;
    }
}

float NanceloidVST::getParameter (VstInt32 index, float value) {
    switch (index) {
        case LUNGS:
            return unmap_from_range (synth->parameters.lungs, -1, 1);
        case GLOTTIS:
            return unmap_from_range (synth->parameters.glottal_tension, -1, 1);
        case LARYNX:
            return unmap_from_range (synth->parameters.laryngeal_height, -1, 1);
        case LIPS:
            return unmap_from_range (synth->parameters.lips_roundedness, 0, 1);
        case JAW:
            return unmap_from_range (synth->parameters.jaw_height, 0, 1);
        case FRONTNESS:
            return unmap_from_range (synth->parameters.tongue_frontness, 0, 1);
        case HEIGHT:
            return unmap_from_range (synth->parameters.tongue_height, 0, 1);
        case FLATNESS:
            return unmap_from_range (synth->parameters.tongue_flatness, -1, 1);
        case VELUM:
            return unmap_from_range (synth->parameters.velic_closure, 0, 1);
        case ACOUSTIC_DAMPING:
            return unmap_from_range (synth->parameters.acoustic_damping, 0, 1);
        case ENUNCIATION:
            return unmap_from_range (synth->parameters.enunciation, 0, 1);
        case PORTAMENTO:
            return unmap_from_range (synth->parameters.portamento, 0, 1);
        case FRICATION:
            return unmap_from_range (synth->parameters.frication, 0, 1);
        case SURFACE_TENSION:
            return unmap_from_range (synth->parameters.surface_tension, 0, 1);
        case TRACT_LENGTH:
            return unmap_from_range (synth->parameters.tract_length, 8, 24);
        case AMBIENT_ADMITTANCE:
            return unmap_from_range (synth->parameters.ambient_admittance, 0, 100);
        case VIBRATO_RATE:
            return unmap_from_range (synth->parameters.vibrato_rate, 0, 16);
        case VIBRATO_DEPTH:
            return unmap_from_range (synth->parameters.vibrato_depth, 0, 2);
        case VELOCITY_WEIGHT:
            return unmap_from_range (synth->parameters.velocity, 0, 1);
        case PANNING:
            return unmap_from_range (synth->parameters.panning, -1, 1);
        case VOLUME:
            return unmap_from_range (synth->parameters.volume, 0, 1);
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
        case LUNGS:
            sprintf (string, "%.2f", synth->parameters.lungs * 100);
            break;
        case GLOTTIS:
            sprintf (string, "%.2f", synth->parameters.glottal_tension * 100);
            break;
        case LARYNX:
            sprintf (string, "%.2f", synth->parameters.laryngeal_height * 100);
            break;
        case LIPS:
            sprintf (string, "%.2f", synth->parameters.lips_roundedness * 100);
            break;
        case JAW:
            sprintf (string, "%.2f", synth->parameters.jaw_height * 100);
            break;
        case FRONTNESS:
            sprintf (string, "%.2f", synth->parameters.tongue_frontness * 100);
            break;
        case HEIGHT:
            sprintf (string, "%.2f", synth->parameters.tongue_height * 100);
            break;
        case FLATNESS:
            sprintf (string, "%.2f", synth->parameters.tongue_flatness * 100);
            break;
        case VELUM:
            sprintf (string, "%.2f", synth->parameters.velic_closure * 100);
            break;
        case ACOUSTIC_DAMPING:
            sprintf (string, "%.2f", synth->parameters.acoustic_damping * 100);
            break;
        case ENUNCIATION:
            sprintf (string, "%.2f", synth->parameters.enunciation * 100);
            break;
        case PORTAMENTO:
            sprintf (string, "%.2f", synth->parameters.portamento * 100);
            break;
        case FRICATION:
            sprintf (string, "%.2f", synth->parameters.frication * 100);
            break;
        case SURFACE_TENSION:
            sprintf (string, "%.2f", synth->parameters.surface_tension * 100);
            break;
        case TRACT_LENGTH:
            sprintf (string, "%.2f", synth->parameters.tract_length);
            break;
        case AMBIENT_ADMITTANCE:
            sprintf (string, "%.2f", synth->parameters.ambient_admittance);
            break;
        case VIBRATO_RATE:
            sprintf (string, "%.2f", synth->parameters.vibrato_rate);
            break;
        case VIBRATO_DEPTH:
            sprintf (string, "%.2f", synth->parameters.vibrato_depth * 100);
            break;
        case VELOCITY_WEIGHT:
            sprintf (string, "%.2f", synth->parameters.velocity * 100);
            break;
        case PANNING:
            sprintf (string, "%.2f", synth->parameters.panning * 100);
            break;
        case VOLUME:
            sprintf (string, "%.2f", synth->parameters.volume * 100);
            break;
    }
}

double NanceloidVST::map_to_range (double value, double min, double max) {
    return min + (max - min) * value;
}

double NanceloidVST::unmap_from_range (double value, double min, double max) {
    return (value - min) / (max - min);
}
