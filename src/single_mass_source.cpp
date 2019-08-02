#include <single_mass_source.h>
#include <cmath>
#include <iostream>

using namespace std;

double SingleMassSource::run (Nanceloid *voice) {
    double vibrato = sin (vibrato_phase * 2 * M_PI) * voice->parameters.vibrato_depth;
    vibrato_phase += voice->parameters.vibrato_rate / voice->rate;
    double frequency = get_frequency (voice->note.note + voice->note.detune + vibrato);
    //double frequency = get_frequency (voice->note.note + voice->note.detune);

    double dt = voice->rate / 1000000.0;
    double e = voice->parameters.lungs;
    double d = fmax (0, -x);

    if (e > 0) {
        double f = frequency - error;
        k += error * 0.000000000000001 * f * f * f;
        //k = frequency * frequency * frequency / 20000000000;
    }
    k = fmax        (0.00000000000001, k);

    double coupling = voice->throat->get_left (0);
    //double a = -k * x * x * x + b * v * e - b * v * v * v + e + d * d * c;
    double a = -k * x - k * B * x * x * x - b * v + e * E + e * (1 - v * v / 3.0) * v + d * d * c;
    v += a * dt + coupling / 16.0;
    x += v * dt;

    // keep it sane y'all
    x = fmin (8, fmax (-8, x));
    v = fmin (8, fmax (-8, v));

    double out = x;

    // pitch detection
    if (e > 0 && out < 0.0 && px > 0.00001) {
        double detected_frequency = 1.0 / (t - pt) * voice->rate;
        pt = t;
        error = frequency - detected_frequency;
    } else if (!(e > 0)) {
        error = 0;
    }

    px = x;
    t += 1;

    return out / 4 * e;
}
