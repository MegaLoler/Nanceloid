#include <iostream>
#include <cmath>
#include <waveguide.h>

using namespace std;

double Waveguide::noise () {
    return rand () / (RAND_MAX / 2.0) - 1;
}

double Waveguide::calculate_gamma (double source_impedance, double target_impedance) {
    if (target_impedance == source_impedance)
        return 0;
    else if (target_impedance == 0 || source_impedance == INFINITY)
        return -1;
    else if (target_impedance == INFINITY || source_impedance == 0)
        return 1;
    else
        return (target_impedance - source_impedance) / (target_impedance + source_impedance);
}

Waveguide::Waveguide (int length, double damping, double turbulence,
                     double left_opening_impedance,
                     double right_opening_impedance)
    : length (length), damping (damping), turbulence (turbulence),
      left_opening_impedance (left_opening_impedance),
      right_opening_impedance (right_opening_impedance) {

    segments = new Segment[length];
}

Waveguide::~Waveguide () {
    delete segments;
}

double Waveguide::get (int i) {
    return segments[i].get ();
}

double Waveguide::get_left (int i) {
    return segments[i].get_left ();
}

double Waveguide::get_right (int i) {
    return segments[i].get_right ();
}

double Waveguide::get_net () {
    double net = 0;
    for (int i = 0; i < length; i++)
        net += segments[i].get ();
    return net;
}

double Waveguide::collect_drain_left () {
    double drain = drain_left;
    drain_left = 0;
    return drain;
}

double Waveguide::collect_drain_right () {
    double drain = drain_right;
    drain_right = 0;
    return drain;
}

void Waveguide::put (int i, double left, double right) {
    segments[i].put (left, right);
}

void Waveguide::prepare () {
    for (int i = 0; i < length; i++) {
        
        // get the impedance for this place, left place, and right place
        double impedance       = segments[i].get_impedance ();
        double left_impedance  = i == 0          ? left_opening_impedance  : segments[i - 1].get_impedance();
        double right_impedance = i == length - 1 ? right_opening_impedance : segments[i + 1].get_impedance();

        // calculate the reflection coefficients from that
        double gamma_left  = calculate_gamma (impedance, left_impedance);
        double gamma_right = calculate_gamma (impedance, right_impedance);

        // and set them
        segments[i].set_gamma (gamma_left, gamma_right);
    }
}

void Waveguide::flush () {
    for (int i = 0; i < length; i++)
        segments[i].flush ();
}

void Waveguide::run () {
    // update the segments
    for (int i = 0; i < length; i++) {

        // this segment
        Segment &segment = segments[i];

        // left and right moving energy
        double left = segment.get_left ();
        double right = segment.get_right ();

        // left and right reflection coefficients
        double gamma_left = segment.get_gamma_left ();
        double gamma_right = segment.get_gamma_right ();

        // calculate left and right reflection
        double reflection_left = left * gamma_left;
        double reflection_right = right * gamma_right;

        // calculate left and right transmission
        double transmission_left = left - reflection_left;
        double transmission_right = right - reflection_right;

        // dampen the reflections
        reflection_left *= 1 - damping;
        reflection_right *= 1 - damping;

        // add turbulence
        // TODO: better turbulence
        double turbulence_left = reflection_left * turbulence * noise ();
        double turbulence_right = reflection_right * turbulence * noise ();

        // put in the reflections with turbulence
        segment.put (reflection_right + turbulence_right, reflection_left + turbulence_left);

        // put left transmission
        if (i == 0)
            drain_left += transmission_left;
        else
            segments[i - 1].put (transmission_left, 0);

        // put right transmission
        if (i == length - 1)
            drain_right += transmission_right;
        else
            segments[i + 1].put (0, transmission_right);
    }

    // flush the changes
    flush ();
}

void Waveguide::debug () {

    cout << "DEBUG WAVEGUIDE:" << endl;

    for (int i = 0; i < length; i++) {

        Segment &segment    = segments[i];
        double left        = segment.get_left ();
        double right       = segment.get_right ();
        double total       = segment.get ();
        double impedance   = segment.get_impedance ();
        double gamma_left  = segment.get_gamma_left ();
        double gamma_right = segment.get_gamma_right ();

        cout << "#" << i << "\tLEFT( " << left << " )"
             << " + RIGHT( " << right << " ) = " << total
             << "\tGAMMA: LEFT( " << gamma_left << " )"
             << ", RIGHT( " << gamma_right << " )"
             << "\t Z( " << impedance << " )" << endl;
    }

    cout << "DRAIN\tLEFT( " << collect_drain_left () << " )"
         << "   RIGHT( " << collect_drain_right () << " )" << endl;
    cout << "NET( " << get_net () << " )" << endl;
    cout << endl;
}
