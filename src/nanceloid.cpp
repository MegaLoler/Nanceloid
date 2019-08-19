#include <nanceloid.h>
#include <iostream>
#include <cmath>

using namespace std;

Nanceloid::~Nanceloid () {
    free ();
}

void Nanceloid::free () {
    if (r != nullptr)
        delete r;
    if (l != nullptr)
        delete l;
    if (r_ != nullptr)
        delete r_;
    if (l_ != nullptr)
        delete l_;
    if (r_junction != nullptr)
        delete r_junction;
    if (l_junction != nullptr)
        delete l_junction;
}

void Nanceloid::set_rate (double rate) {
    if (rate != this->rate) {
        this->rate = rate * super_sampling;
        control_rate = this->rate / control_rate_divider;
        dt = 1 / this->rate;
        init ();
    }
}

void Nanceloid::run (float *out) {
    // run super samples
    // TODO: fix super samples whe its > 1 ????
    double output = 0;
    for (int i = 0; i < super_sampling; i++) {
        // run control rate operations
        if (clock++ % control_rate_divider == 0)
            run_control ();

        // cheap filter to smooth pops
        double weight = 1 / (pressure_smoothing + 1);
        pressure = (target_pressure * weight + pressure) / (1 + weight);

        // glottal source
        // mass spring damper system
        double k = w * w;
        double a = -x * k - 1000 * x * x * x * k - v * x * x * w / 100;
        v += a * dt + pressure * 200 - l[0];
        x += v * dt;
        double target_osc = pressure * x * x * w / 200;
        filter = (filter + target_osc - osc) / 1.001;
        osc = target_osc;
        cout << filter << endl;

        // update ends of waveguide
        int end = waveguide_length - 1;
        r_[0]   = l[0]   * l_junction[0] + filter;
        l_[end] = r[end] * r_junction[end];

        // update inner of waveguide
        for (int j = 0; j < waveguide_length; j++) {
            int j0 = j;
            int j1 = j + 1;
            double r_refl = r[j0] * r_junction[j0];
            double l_refl = l[j1] * l_junction[j1];
            r_[j1] = r[j0] - r_refl + l_refl;
            l_[j0] = l[j1] - l_refl + r_refl;
        }

        // swap buffers
        double *r__ = r;
        double *l__ = l;
        r = r_;
        l = l_;
        r_ = r__;
        l_ = l__;

        // accumulate sound output from right end of waveguide
        output += r[waveguide_length - 1];
    }

    // mix and return the samples
    double target_sample = output / super_sampling * params.volume.value;   // output volume
    double pan = params.panning.get_normalized_value () / 2;                // panning
    sample = (target_sample + sample) / 2;                                  // cheap filter
    out[0] = cos (pan * M_PI) * sample;
    out[1] = sin (pan * M_PI) * sample;
}

void Nanceloid::run_control () {
    // run tremolo lfo
    tremolo_osc = 1 - (sin (tremolo_phase * M_PI * 2) + 1) / 2 * params.tremolo_depth.value;
    tremolo_phase += params.tremolo_rate.value / control_rate;

    // run vibrato lfo
    vibrato_osc = sin (vibrato_phase * M_PI * 2) * params.vibrato_depth.value;
    vibrato_phase += params.vibrato_rate.value / control_rate;

    // run adsr envelope to get current input pressure
    double delta_clock = clock - note.start_time;  // samples since note event
    double delta_time = delta_clock / rate;        // seconds since note event
    double sustain = params.adsr_sustain.value * note.velocity
        * (1 - params.min_velocity.value) + params.min_velocity.value;  // effective sustain level
    sustain *= tremolo_osc;
      
    target_pressure = 0;
    if (note.note) {
        if (note.velocity) {
            // note is on
            if (delta_time < params.adsr_attack.value)
                // attack
                target_pressure = delta_time / params.adsr_attack.value;
            else if (delta_time < params.adsr_attack.value + params.adsr_decay.value)
                // decay
                target_pressure = 1 - (1 - sustain) * (delta_time - params.adsr_attack.value) / params.adsr_decay.value;
            else
                // sustain
                target_pressure = sustain;
        } else {
            // note is off
            if (delta_time < params.adsr_attack.value)
                // release
                target_pressure = sustain - sustain * delta_time / params.adsr_release.value;
        }
    }

    // update target frequency
    double semitones = note.note + note.detune + vibrato_osc;
    frequency = 440 * pow (2.0, (semitones - 69) / 12);

    // pitch correction
    w = frequency;

    // update shape
    // TODO: approach target shape
    update_reflections ();
}

void Nanceloid::init () {
    // free old waveguide
    free ();

    // calculate number of segments based on desired length
    waveguide_length = (int) floor (params.tract_length.value * rate / speed_of_sound);

    // create the new arrays
    r = new double[waveguide_length];
    l = new double[waveguide_length];
    r_ = new double[waveguide_length];
    l_ = new double[waveguide_length];
    r_junction = new double[waveguide_length];
    l_junction = new double[waveguide_length];

    // precalculate reflection coefficients
    update_reflections ();
}

void Nanceloid::update_reflections () {
    // calculate the segment impedances
    double impedance[waveguide_length];
    for (int i = 0; i < waveguide_length; i++) {
        double n = i / (waveguide_length - 1);
        double diameter = shape.sample (n);
        double area = diameter * 2; // works ig lol
        impedance[i] = 1 / area;
    }
    // calculate right going coefficients
    for (int i = 0; i < waveguide_length - 1; i++) {
        double z0 = impedance[i];
        double z1 = impedance[i + 1];
        r_junction[i] = (z1 - z0) / (z1 + z0);
    }
    // calculate left going coefficients
    for (int i = 1; i < waveguide_length; i++) {
        double z0 = impedance[i];
        double z1 = impedance[i - 1];
        l_junction[i] = (z1 - z0) / (z1 + z0);
    }
    // update end reflections
    r_junction[waveguide_length - 1] = params.refl_right.value;
    l_junction[0] = params.refl_left.value;
}

void Nanceloid::midi (uint8_t *data) {

    // parse the data
    uint8_t type = data[0] & 0xf0;
    uint8_t chan = data[0] & 0x0f;
    
#ifdef DEBUG
    cout << "Received midi event: 0x" << hex << (int) type << " channel: 0x" << hex << (int) chan << endl;
#endif

    if (type == 0xb0) {

        // handle control events
        uint8_t id = data[1];
        uint8_t value = data[2];

#ifdef DEBUG
        cout << "Received midi controller event: 0x" << hex << (int) id << " 0x" << hex << (int) value << endl;
#endif

        // update parameters mapped to this cc
        Parameter *array = params.as_array ();
        for (int i = 0; i < params.length (); i++ ) {
            Parameter &p = array[i];
            if (p.is_mapped (id))
                p.set_midi_value (value);
        }

#ifdef DEBUG
        params.print ();
#endif

    } else if (type == 0x80) {

        // handle note off events
        uint8_t note = data[1];

        if (note == this->note.note) {

#ifdef DEBUG
            cout << "Received midi note off event: 0x" << hex << (int) note << endl;
#endif

            this->note.velocity = 0;
            this->note.start_time = clock;
        }

    } else if (type == 0x90) {

        // handle note on events
        uint8_t note = data[1];
        uint8_t velocity = data[2];

#ifdef DEBUG
        cout << "Received midi note on event: 0x" << hex << (int) note << " 0x" << hex << (int) velocity << endl;
#endif

        this->note.note = note;
        this->note.velocity = velocity / 127.0;
        this->note.start_time = clock;

    } else if (type == 0xe0) {

        // handle pitch bends
        uint8_t msb = data[2];

#ifdef DEBUG
        cout << "Received pitch bend: 0x" << hex << (int) msb << endl;
#endif

        this->note.detune = msb / 127.0 * 4 - 2;

    }

    // TODO: handle phoneme mapped notes on channel 10 or w/e
}
