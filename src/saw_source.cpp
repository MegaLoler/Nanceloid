#include <saw_source.h>

double SawSource::run (Nanceloid *voice) {

    // musical dynamics
    // TODO: make vibrato control rate
    double vibrato = sin (vibrato_phase * 2 * M_PI) * voice->parameters.vibrato_depth;
    vibrato_phase += voice->parameters.vibrato_rate / voice->rate;

    double target_frequency = get_frequency (voice->note.note + voice->note.detune + vibrato);
    frequency += (target_frequency - frequency) * voice->parameters.portamento * portamento;

    // saw osc
    double saw = osc_phase;
    osc_phase += frequency / voice->rate;
    osc_phase = fmod (osc_phase, 1);

    // intensity depending on glottal tension
    double intensity = voice->parameters.lungs * (voice->parameters.glottal_tension + 1) / 2;
    double loudness = voice->parameters.lungs * (fmin (voice->parameters.glottal_tension, 0) + 1);

    // filter
    double sample = osc;
    double delta = saw - sample;
    double k = pow (intensity, 3);
    osc += delta * k;

    return sample * loudness;
}
