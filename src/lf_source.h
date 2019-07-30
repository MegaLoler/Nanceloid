#pragma once

#include <glottal_source.h>

class LFSource : public GlottalSource {
    private:
        double t = 0;

    public:
        double run (Nanceloid *voice);
};
