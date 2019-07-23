#include <iostream>
#include <waveguide.h>

using namespace std;

static double Waveguide::calculate_gamma (double source_impedance, double target_impedance) {
    return (target_impedance - source_impedance) / (target_impedance + source_impedance);
}

Waveguid::Waveguide (int length, double damping = 0.04, double turbulence = 0.1,
                     double left_opening_impedance = INFINITY,
                     double right_opening_impedance = 0.1)
    : length (length), damping (damping), turbulence (turbulence),
      left_opening_impedance (left_opening_impedance),
      right_opening_impedance (right_opening_impedance);

    segments = new Segment[length];
}

Waveguide::~Waveguide () {
    delete segments;
}

double Waveguide::get (int i) {
    segments[i].get ();
}

double Waveguide::get_left (int i) {
    segments[i].get_left ();
}

double Waveguide::get_right (int i) {
    segments[i].get_right ();
}

double Waveguide::get_net () {
    double net = 0;
    for (int i = 0; i < length; i++)
        net += segments[i].get (i);
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
        double left_impedance  = i == 0          ? left_opening_impedance  : segments[i - 1].get_impedance()
        double right_impedance = i == length - 1 ? right_opening_impedance : segments[i + 1].get_impedance()

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
        Segment segment = segments[i];

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

        // put the reflections
        segment.put (reflection_right, reflection_left);

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
        double left = get_left (i);
        double right = get_right (i);
        double total = get (i);
        cout << "#" << i << "\tLEFT=" << left << " +\tRIGHT=" << right << " = " << total << endl;
    }

    cout << "DRAIN\tLEFT=" << collect_drain_left () << "\tRIGHT=" << collect_drain_right () << endl;
    cout << "NET=" << get_net () << endl;
    cout << endl;
}
