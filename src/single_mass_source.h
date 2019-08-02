#pragma once

#include <glottal_source.h>

class SingleMassSource : public GlottalSource {
    private:
        double k = 1;           // spring tension
        double b = 0.1;         // damping
        double c = 1000;        // collision
        double B = 10;          // nonlinear coefficient
        double E = 0.1;         // excitation

        double x = 0;           // mass displacement
        double v = 0;           // mass velocity

        double vibrato_phase = 0;   // vibrato lfo current phase

        // for pitch detection/correction
        double error = 0;
        double t = 0;
        double pt = 0;
        double px = 0;

    public:
        double run (Nanceloid *voice);
};
