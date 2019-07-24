#include <saw_source.h>

double SawSource::run (Nanceloid *voice) {

    // saw osc
    double saw = voice->osc_phase;
    voice->osc_phase += voice->parameters.frequency / voice->rate;
    voice->osc_phase = fmod (voice->osc_phase, 1);

    // intensity depending on glottal tension
    double intensity = voice->parameters.lungs * (voice->parameters.glottal_tension + 1) / 2;
    double loudness = voice->parameters.lungs * (fmin (voice->parameters.glottal_tension, 0) + 1);

    // filter
    double sample = voice->osc;
    double delta = saw - sample;
    double k = pow (intensity, 3);
    voice->osc += delta * k;

    return sample * loudness;
}
