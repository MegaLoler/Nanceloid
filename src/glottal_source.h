#pragma once

#include <nanceloid.h>

class Nanceloid;

class GlottalSource {
    protected:
        // return 12tet frequency for a given midi note value
        double get_frequency (double note) {
            return 440.0 * pow (2.0, (note - 69) / 12);
        }

    public:
        // run the glottal source one step and return the next sample
        virtual double run (Nanceloid *voice) = 0;
};
