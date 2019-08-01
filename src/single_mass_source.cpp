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
    double d = fmax (0, 0-x);
    double e = voice->parameters.lungs - 0.1;

    if (e > 0) {
        double f = frequency - error;
        //k += error * 0.0000000000001 * f * f * f;
        k = frequency * frequency * frequency / 200000000;
    }
    k = fmax (0.0001, k);

    double coupling = voice->throat->get_left (0);
    double a = -k * x * x * x + b * v * e - b * v * v * v + e + d * d * c;
    v += a * dt + coupling / 2;
    x += v * dt;

    x = fmin (8, fmax (-8, x));
    v = fmin (8, fmax (-8, v));

    // high pass filter
    double out = x;
    double y = px_;
    px_ = out;
    out = (rc / (rc + dt)) * (px + out - y);

    // pitch detection
    if (e > 0 && out < 0.1 && px > 0.10001) {
        double detected_frequency = 1.0 / (t - pt) * voice->rate;
        pt = t;
        error = frequency - detected_frequency;
    } else if (!(e > 0)) {
        error = 0;
    }

    px = out;
    t += 1;

    return out * e / 4;
}
