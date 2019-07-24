#pragma once

#include <glottal_source.h>

class SawSource : public GlottalSource {
    public:
        double run (Nanceloid *voice);
}
