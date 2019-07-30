#include <lf_source.h>

double LFSource::run (Nanceloid *voice) {
    // taken from pink trombone lol
    // TODO: optimise for real
    double frequency = get_frequency (voice->note.note + voice->note.detune);
    double tenseness = (voice->parameters.glottal_tension + 1) / 2;
    double Rd = 3*(1-tenseness);
    if (Rd<0.5) Rd = 0.5;
    if (Rd>2.7) Rd = 2.7;
    double Ra = -0.01 + 0.048*Rd;
    double Rk = 0.224 + 0.118*Rd;
    double Rg = (Rk/4)*(0.5+1.2*Rk)/(0.11*Rd-Ra*(0.5+1.2*Rk));
    double Ta = Ra;
    double Tp = 1. / (2*Rg);
    double Te = Tp + Tp*Rk;
    double epsilon = 1./Ta;
    double shift = pow(M_E, -epsilon * (1.-Te));
    double Delta = 1. - shift; //divide by this to scale RHS
    double RHSIntegral = (1/epsilon)*(shift - 1) + (1-Te)*shift;
    RHSIntegral = RHSIntegral/Delta;
    double totalLowerIntegral = - (Te-Tp)/2 + RHSIntegral;
    double totalUpperIntegral = -totalLowerIntegral;
    double omega = M_PI/Tp;
    double s = sin(omega*Te);
    double y = -M_PI*s*totalUpperIntegral / (Tp*2);
    double z = log(y);
    double alpha = z/(Tp/2 - Te);
    double E0 = -1 / (s*pow(M_E, alpha*Te));
    double output;
    if (t>Te) output = (-pow(M_E, -epsilon * (t-Te)) + shift)/Delta;
    else output = E0 * pow(M_E, alpha*t) * sin(omega * t);
    double intensity = voice->parameters.lungs * tenseness;
    double loudness = voice->parameters.lungs * (fmin (voice->parameters.glottal_tension, 0) + 1);
    t += 1.0 / voice->rate * frequency;
    while (t > 1) t -= 1;
    return output * intensity * loudness;
}
