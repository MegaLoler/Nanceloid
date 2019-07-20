#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <math.h>
#define PI 3.1415926535897

class NN_VST : public AudioEffectX
{
public:
  NN_VST(audioMasterCallback audioMaster);
  ~NN_VST() {};
  
  virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames);
  virtual VstInt32 processEvents(VstEvents* events);
  
  virtual VstInt32 canDo(char* text);
  virtual VstInt32 getNumMidiInputChannels();
  virtual VstInt32 getNumMidiOutputChannels();
  virtual void setSampleRate(float sampleRate);

  virtual VstInt32 getVendorVersion();
  virtual bool getEffectName(char*);
  virtual bool getProductString(char*);
  virtual bool getVendorString(char*);

  virtual void setParameter(VstInt32, float);
  virtual float getParameter(VstInt32, float);
  virtual void getParameterName(VstInt32, char*);
  virtual void getParameterLabel(VstInt32, char*);
  virtual void getParameterDisplay(VstInt32, char*);
};

enum {
  PLACEHOLDER,

  PARAMETER_COUNT
};
const char* paramNames[PARAMETER_COUNT] = {
  "Placeholder",
};
const char* paramLabels[PARAMETER_COUNT] = {
  "%"
};
