#pragma once

#include <glottal_source.h>

class SingleMassSource : public GlottalSource {
    private:
        double k = 1;           // spring tension
        double b = 0.5;           // damping
        double c = 1000;          // collision strength

        double x = 0;           // mass displacement
        double v = 0;           // mass velocity

        double vibrato_phase = 0;   // vibrato lfo current phase

        double rc = 1;        // high pass filter

        // for pitch detection/correction
        double error = 0;
        double t = 0;
        double pt = 0;
        double px_ = 0;
        double px = 0;

    public:
        double run (Nanceloid *voice);
};
