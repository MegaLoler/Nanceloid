#include "vst.h"
#include <cstdio>

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
  return new NN_VST(audioMaster);
}

NN_VST::NN_VST(audioMasterCallback audioMaster)
  : AudioEffectX(audioMaster, 0, PARAMETER_COUNT)
{
  setNumInputs(0);
  setNumOutputs(2);
  setUniqueID('nanc');
  canProcessReplacing();
  isSynth();

  //init_synth();
}

// TODO: deconstructor, make it call free_tract()

VstInt32 NN_VST::canDo(char* text)
{
  if (!strcmp(text, "receiveVstEvents"))
    return 1;
  if (!strcmp(text, "receiveVstMidiEvent"))
    return 1;
  return -1;  // explicitly can't do; 0 => don't know
}

VstInt32 NN_VST::getNumMidiInputChannels()
{
  return 1; // or up to 16 if you prefer
}

VstInt32 NN_VST::getNumMidiOutputChannels()
{
  return 0; // no MIDI output back to host
}

void NN_VST::setSampleRate(float sampleRate)
{
  AudioEffectX::setSampleRate(sampleRate);
 // rate = sampleRate;
//  init_tract(tract_length);
}

void NN_VST::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
  float* out1 = outputs[0];
  float* out2 = outputs[1];

  while (sampleFrames--) {
    (*out1++) = (*out2++) = 0;//run_tract(glottal_source());
    //(*out++) = (*in++);
  }
}

VstInt32 NN_VST::processEvents(VstEvents* ev)
{
    for (VstInt32 i = 0; i < ev->numEvents; i++) {
        if ((ev->events[i])->type != kVstMidiType)
            continue;

        VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
        char* midiData = event->midiData;
        VstInt32 type = midiData[0] & 0xf0;
        VstInt32 chan = midiData[0] & 0x0f;

        // control signal
        if(type == 0xb0) {
            VstInt32 id = midiData[1];
            VstInt32 value = midiData[2];
        }
        else if(type == 0x80) {
            //uint8_t note = event.buffer[1];
            //uint8_t velocity = event.buffer[2];
            //printf("  [chan %02d] midi note OFF: 0x%x, 0x%x\n", chan, note, velocity);
            //struct Phoneme *phoneme = get_mapped_phoneme(note);
            //if (target_phoneme == phoneme) {
            //    // TODO: a stack of notes to return to
            //    target_phoneme = &ambient_phoneme;
            //}
        }
        else if(type == 0x90) {
            VstInt32 note = midiData[1];
            VstInt32 velocity = midiData[2];
            //printf("  [chan %02d] midi note ON:  0x%x, 0x%x\n", chan, note, velocity);
            //target_phoneme = get_mapped_phoneme(note);
            //ambient_phoneme = *get_mapped_phoneme(note);

            //// TMP: insert plosive transient
            //if(note == 0x30) {
            //    segments_front[nsegments-1].right += diaphram_pressure;
            //}
            //
        }
    }
    return 1;
}


VstInt32 NN_VST::getVendorVersion()
{
  return 1000;
}

bool NN_VST::getEffectName(char* name)
{
  vst_strncpy(name, "NN_VST sneek peek", kVstMaxEffectNameLen);
  return true;
}

bool NN_VST::getProductString(char* text)
{
  vst_strncpy(text, "NN_VST sneek peek", kVstMaxProductStrLen);
  return true;
}

bool NN_VST::getVendorString(char* text)
{
  vst_strncpy(text, "Negative Nancy Duh", kVstMaxVendorStrLen);
  return true;
}

void NN_VST::setParameter(VstInt32 index, float value)
{
  switch (index) {
    case PLACEHOLDER:
        // placeholder
        break;
  }
}

float NN_VST::getParameter(VstInt32 index, float value)
{
  switch (index) {
    case PLACEHOLDER:
        return 0;
        break;
    default:
        return 0;
  }
}

void NN_VST::getParameterName(VstInt32 index, char* label)
{
  vst_strncpy(label, paramNames[index], kVstMaxParamStrLen);
}

void NN_VST::getParameterLabel(VstInt32 index, char* label)
{
  vst_strncpy(label, paramLabels[index], kVstMaxParamStrLen);
}

void NN_VST::getParameterDisplay(VstInt32 index, char* text)
{
  switch (index) {
    case PLACEHOLDER:
        sprintf(text, "placeholder");
        break;
  }
}
