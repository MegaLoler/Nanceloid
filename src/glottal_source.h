#pragma once

#include <nanceloid.h>

class Nanceloid;

class GlottalSource {
    public:
        virtual double run (Nanceloid *voice) = 0;
};
