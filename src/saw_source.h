#pragma once

#include <glottal_source.h>

class SawSource : public GlottalSource {
    private:
        double osc = 0;             // last oscillator sample
        double osc_phase = 0;       // glottal oscillator current phase
        double vibrato_phase = 0;   // vibrato lfo current phase
        double frequency = 0;       // current oscillating frequency

    public:
        double run (Nanceloid *voice);
};
