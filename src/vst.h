#include <audioeffectx.h>
#include <waveguide.h>
#include <math.h>

class NN_VST : public AudioEffectX {
    public:
        NN_VST (audioMasterCallback audio_master);
        ~NN_VST ();

        virtual void processReplacing (float **inputs, float **outputs, VstInt32 frames);
        virtual VstInt32 processEvents (VstEvents *events);

        virtual VstInt32 canDo (char *text);
        virtual VstInt32 getNumMidiInputChannels ();
        virtual VstInt32 getNumMidiOutputChannels ();
        virtual void setSampleRate (float rate);

        virtual VstInt32 getVendorVersion ();
        virtual bool getEffectName (char *name);
        virtual bool getProductString (char *string);
        virtual bool getVendorString (char *string);

        virtual void setParameter (VstInt32 index, float value);
        virtual float getParameter (VstInt32 index, float value);
        virtual void getParameterName (VstInt32 index, char *name);
        virtual void getParameterLabel (VstInt32 index, char *label);
        virtual void getParameterDisplay (VstInt32 index, char *string);

    private:
        NN_Waveguide *waveguide;
        NN_Node *source_node;
        NN_Node *drain_node;

        void init_waveguide ();
};

enum PARAMETER_TYPE {
    PLACEHOLDER,

    PARAMETER_COUNT
};

const char *parameter_names[PARAMETER_COUNT] = {
    "Placeholder"
};
const char *parameter_labels[PARAMETER_COUNT] = {
    "%"
};
