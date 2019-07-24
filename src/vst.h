#include <audioeffectx.h>
#include <nanceloid.h>
#include <saw_source.h>
#include <cmath>

class NanceloidVST : public AudioEffectX {
    public:
        NanceloidVST (audioMasterCallback audio_master);
        ~NanceloidVST ();

        void processReplacing (float **inputs, float **outputs, VstInt32 frames);
        VstInt32 processEvents (VstEvents *events);

        VstInt32 canDo (char *text);
        VstInt32 getNumMidiInputChannels ();
        VstInt32 getNumMidiOutputChannels ();
        void setSampleRate (float rate);

        VstInt32 getVendorVersion ();
        bool getEffectName (char *name);
        bool getProductString (char *string);
        bool getVendorString (char *string);

        void setParameter (VstInt32 index, float value);
        float getParameter (VstInt32 index, float value);
        void getParameterName (VstInt32 index, char *name);
        void getParameterLabel (VstInt32 index, char *label);
        void getParameterDisplay (VstInt32 index, char *string);

    private:
        Nanceloid *synth;

        // map a normalized value to a given range
        double map_to_range (double value, double min, double max);

        // return a normalized value given a range
        double unmap_from_range (double value, double min, double max);
};

enum PARAMETER_TYPE {
    LUNGS,
    GLOTTIS,
    LARYNX,
    LIPS,
    JAW,
    FRONTNESS,
    HEIGHT,
    FLATNESS,
    VELUM,
    ACOUSTIC_DAMPING,
    ENUNCIATION,
    PORTAMENTO,
    FRICATION,
    SURFACE_TENSION,
    TRACT_LENGTH,
    AMBIENT_ADMITTANCE,
    VIBRATO_RATE,
    VIBRATO_DEPTH,
    VELOCITY_WEIGHT,
    PANNING,
    VOLUME,

    PARAMETER_COUNT
};

const char *parameter_names[PARAMETER_COUNT] = {
    "Lungs",
    "Glottis",
    "Larynx",
    "Lips",
    "Jaw",
    "Position",
    "Height",
    "Flatness",
    "Velum",
    "Damping",
    "Enunc.",
    "Portam.",
    "Friction",
    "Tension",
    "Length",
    "Amb. Y",
    "Vib Rate",
    "Vib Dep.",
    "Velocity",
    "Panning",
    "Volume",
};

const char *parameter_labels[PARAMETER_COUNT] = {
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "%",
    "cm",
    "℧",
    "hz",
    "cents",
    "%",
    "%",
    "%",
};
