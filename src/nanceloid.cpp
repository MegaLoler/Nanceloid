#include <nanceloid.h>
#include <iostream>
#include <cmath>

using namespace std;

double noise () {
    return (double) rand () / RAND_MAX;
}

double clip (double value) {
    return fmin (5, fmax (-5, value));
}

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
    if (nr != nullptr)
        delete nr;
    if (nl != nullptr)
        delete nl;
    if (nr_ != nullptr)
        delete nr_;
    if (nl_ != nullptr)
        delete nl_;
}

void Nanceloid::set_rate (double rate) {
    if (rate != this->rate) {
        this->rate = rate * super_sampling;
        control_rate = this->rate / control_rate_divider;
        dt = 1.0 / rate;
        init ();
    }
}

double Nanceloid::get_frequency () {
    return frequency;
}

double Nanceloid::get_voicing () {
    return voicing;
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

        // glottal source and uvula
        const double amp = 0.1;
        const double damping = 0.1;
        const double tract_coupling = 0.1;  // folds coulping to resonator
        const double uvula_tract_coupling = 0.5; // uvula couplng to resonator
        const double fold_coupling_k = 1 * cord_tension / 2;
        const double n = 10;
        const double nd = 5;
        const double uvula = params.uvula.value;
        const double fold_2_c = params.second_fold.value; // how present the second simulated fold is
        const double uvula_frequency = 100;
        const double uvula_tension = pow (uvula_frequency * 2 * M_PI, 2.0);
        double coupling_spring = fold_coupling_k * (x2 - x);
        // first fold
        double delta_pressure = pressure + l[0] * tract_coupling;
        double a = -cord_tension * (x + n * x * x * x) - damping * (v + nd * v * x * x) * frequency + delta_pressure * amp * cord_tension * (1 + n) + coupling_spring;
        // second fold
        double delta_pressure2 = (r[0] + l[1]) * tract_coupling;
        double a2 = -cord_tension * (x2 + n * x2 * x2 * x2) - damping * (v2 + nd * v2 * x2 * x2) * frequency + delta_pressure2 * amp * cord_tension * (1 + n) - coupling_spring;
        // uvula
        int ui = mouth_i + 1;
        double delta_pressure3 = (r[ui] + l[ui + 1]) * uvula_tract_coupling;
        double a3 = -uvula_tension * (x3 + n * x3 * x3 * x3) - damping * (v3 + nd * v3 * x3 * x3) * uvula_frequency + delta_pressure3 * amp * uvula_tension * (1 + n);
        // integrate
        v  += a  * dt;
        v2 += a2 * dt;
        v3 += a3 * dt;
        x  += v  * dt;
        x2 += v2 * dt;
        x3 += v3 * dt;
        // update waveguide
        // first fold
        shape.set_sample (0, fmax (0, x));
        // second fold
        double i2 = 1.0 / (waveguide_length - 1);
        double x2_ = (shape.sample (i2) + x2 * fold_2_c) / (1 + fold_2_c);
        shape.set_sample (i2, fmax (0, x2_));
        // uvula
        double i3 = (double) ui / (waveguide_length - 1);
        double x3_ = shape.sample (i3) + x3 * uvula;
        shape.set_sample (i3, fmax (0, x3_));
        // update reflection coefficients
        double z0 = get_impedance (0);
        double z1 = get_impedance (1);
        double z2 = get_impedance (2);
        double zu0 = get_impedance (ui);
        double zu1 = get_impedance (ui + 1);
        r_junction[0] = z1 > max_impedance ? 1 : (z1 - z0) / (z1 + z0);
        l_junction[1] = z0 > max_impedance ? 1 : (z0 - z1) / (z0 + z1);
        r_junction[1] = z2 > max_impedance ? 1 : (z2 - z1) / (z2 + z1);
        l_junction[2] = z1 > max_impedance ? 1 : (z1 - z2) / (z1 + z2);
        r_junction[ui]     = zu1 > max_impedance ? 1 : (zu1 - zu0) / (zu1 + zu0);
        l_junction[ui + 1] = zu0 > max_impedance ? 1 : (zu0 - zu1) / (zu0 + zu1);
        // glottal output
        double disp = pow (x + 1 - voicing, 2.0) * M_PI;
        double glottal_output = pressure * disp;

        // update ends of waveguide
        int end = waveguide_length - 1;
        int nose_end = nose_length - 1;
        r_[0]   = l[0]   * l_junction[0] + glottal_output;
        l_[end] = r[end] * r_junction[end];
        nl_[nose_end] = nr[nose_end] * params.refl_right.value;

        // update nose throat mouth junction
        double refl_c = 1 - reflection_damping;
        double throat_out = r[throat_i];
        double mouth_out = l[mouth_i];
        double nose_out = nl[0];
        double throat_refl = throat_refl_c * throat_out;
        double mouth_refl = mouth_refl_c * mouth_out;
        double nose_refl = nose_refl_c * nose_out;
        double throat_trans = throat_out - throat_refl;
        double mouth_trans = mouth_out - mouth_refl;
        double nose_trans = nose_out - nose_refl;
        double throat_to_mouth = throat_to_mouth_w * throat_trans;
        double throat_to_nose = throat_to_nose_w * throat_trans;
        double mouth_to_throat = mouth_to_throat_w * mouth_trans;
        double mouth_to_nose = mouth_to_nose_w * mouth_trans;
        double nose_to_throat = nose_to_throat_w * nose_trans;
        double nose_to_mouth = nose_to_mouth_w * nose_trans;
        double throat_in = mouth_to_throat + nose_to_throat + throat_refl * refl_c;
        double mouth_in = throat_to_mouth + nose_to_mouth + mouth_refl * refl_c;
        double nose_in = throat_to_nose + mouth_to_nose + nose_refl * refl_c;
        l_[throat_i] = throat_in;
        r_[mouth_i] = mouth_in;
        nr_[0] = nose_in;

        // update mouth and throat
        for (int j = 0; j < waveguide_length - 1; j++) {
            // skip the nose throat mouth junction
            // since it was handled up there
            if (j == throat_i)
                continue;
            int j0 = j;
            int j1 = j + 1;
            double r_refl = r[j0] * r_junction[j0];
            double l_refl = l[j1] * l_junction[j1];
            // TODO: flow turbelence
            double r_turb = 0;//fmax (0, r_refl) * params.turbulence.value * noise ();
            double l_turb = 0;//fmax (0, l_refl) * params.turbulence.value * noise ();
            r_[j1] = clip (r[j0] - r_refl + l_refl * refl_c + l_turb);
            l_[j0] = clip (l[j1] - l_refl + r_refl * refl_c + r_turb);
        }
        // update nose
        for (int j = 0; j < nose_length - 1; j++) {
            int j0 = j;
            int j1 = j + 1;
            nr_[j1] = clip (nr[j0]);
            nl_[j0] = clip (nl[j1]);
        }

        // swap buffers
        double *r__ = r;
        double *l__ = l;
        r = r_;
        l = l_;
        r_ = r__;
        l_ = l__;
        double *nr__ = nr;
        double *nl__ = nl;
        nr = nr_;
        nl = nl_;
        nr_ = nr__;
        nl_ = nl__;

        // accumulate sound output from right end of waveguide
        double mouth_radiance = 1 - r_junction[end];
        double nose_radiance = 1 - params.refl_right.value;
        double mouth_output = r[end] * mouth_radiance;
        double nose_output = nr[nose_end] * nose_radiance;
        output += (mouth_output + nose_output);
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
    double sustain = params.adsr_sustain.value;    // effective sustain level
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
    target_pressure *= note.velocity * (1 - params.min_velocity.value) + params.min_velocity.value;

    // crossfade voicing
    voicing += (params.voicing.value - voicing) * params.crossfade.value;

    // update target frequency
    double semitones = note.note + note.detune + vibrato_osc;
    double target_frequency = 440 * pow (2.0, (semitones - 69) / 12);
    frequency += (target_frequency - frequency) * params.portamento.value;

    // pitch correction
    // TODO: actually do it
    cord_tension = pow (frequency * 2 * M_PI, 2.0);

    // update shape
    shape.crossfade (get_shape (), params.crossfade.value);
    update_reflections ();
}

void Nanceloid::init () {
    // free old waveguide
    free ();

    // calculate number of segments based on desired length
    // +2 for the 2 vocal fold segments
    waveguide_length = (int) floor (params.tract_length.value * rate / speed_of_sound) + 2;
    nose_length = waveguide_length / 2;

    // indices of throat and mouth at junction
    throat_i = waveguide_length - nose_length - 1;
    mouth_i = throat_i + 1;

    // create the new arrays
    r = new double[waveguide_length];
    l = new double[waveguide_length];
    r_ = new double[waveguide_length];
    l_ = new double[waveguide_length];
    r_junction = new double[waveguide_length];
    l_junction = new double[waveguide_length];
    nr = new double[nose_length];
    nl = new double[nose_length];
    nr_ = new double[nose_length];
    nl_ = new double[nose_length];

    // clear them
    for (int i = 0; i < waveguide_length; i++) {
        r[i] = l[i] = r_[i] = l_[i] = r_junction[i] = l_junction[i] = 0;
    }
    for (int i = 0; i < nose_length; i++) {
        nr[i] = nl[i] = nr_[i] = nl_[i] = 0;
    }

    // precalculate reflection coefficients
    update_reflections ();
}

TractShape &Nanceloid::get_shape () {
    return shapes[shape_i];
}

int Nanceloid::get_shape_id () {
    return shape_i;
}

void Nanceloid::set_shape_id (int id) {
    shape_i = id;
}

double Nanceloid::get_impedance (int i) {
    double n = (double) i / (waveguide_length - 1);
    double diameter = shape.sample (n);
    double radius = diameter / 2;
    double area = radius * radius * M_PI;
    return 1 / (area + epsilon);
}

void Nanceloid::update_reflections () {
    // calculate the segment impedances
    double impedance[waveguide_length];
    for (int i = 0; i < waveguide_length; i++) {
        impedance[i] = get_impedance (i);
    }
    // calculate right going coefficients
    for (int i = 0; i < waveguide_length - 1; i++) {
        double z0 = impedance[i];
        double z1 = impedance[i + 1];
        r_junction[i] = z1 > max_impedance ? 1 : (z1 - z0) / (z1 + z0);
    }
    // calculate left going coefficients
    for (int i = 1; i < waveguide_length; i++) {
        double z0 = impedance[i];
        double z1 = impedance[i - 1];
        l_junction[i] = z1 > max_impedance ? 1 : (z1 - z0) / (z1 + z0);
    }
    // update end reflections
    r_junction[waveguide_length - 1] = params.refl_right.value;
    l_junction[0] = params.refl_left.value;
    // throat mouth nose junction reflection coefficients and transmittance weights
    double throat_z = get_impedance (throat_i);
    double mouth_z = get_impedance (mouth_i);
    double nose_z = 1.0 / (params.nose_admittance.value * (1 - shape.velic_closure) + epsilon);
    double throat_y = 1.0 / throat_z;
    double mouth_y = 1.0 / mouth_z;
    double nose_y = 1.0 / nose_z;
    double mouth_nose_y = mouth_y + nose_y;
    double throat_nose_y = throat_y + nose_y;
    double throat_mouth_y = throat_y + mouth_y;
    double mouth_nose_z = 1.0 / mouth_nose_y;
    double throat_nose_z = 1.0 / throat_nose_y;
    double throat_mouth_z = 1.0 / throat_mouth_y;
    throat_refl_c = (mouth_nose_z - throat_z) / (mouth_nose_z + throat_z);
    mouth_refl_c = (throat_nose_z - mouth_z) / (throat_nose_z + mouth_z);
    nose_refl_c = (throat_mouth_z - nose_z) / (throat_mouth_z + nose_z);
    throat_to_mouth_w = mouth_y / mouth_nose_y;
    throat_to_nose_w = nose_y / mouth_nose_y;
    mouth_to_throat_w = throat_y / throat_nose_y;
    mouth_to_nose_w = nose_y / throat_nose_y;
    nose_to_throat_w = throat_y / throat_mouth_y;
    nose_to_mouth_w = mouth_y / throat_mouth_y;
}

void Nanceloid::note_on (int note, double velocity) {
    this->note.note = note;
    this->note.velocity = velocity;
    this->note.start_time = clock;
}

void Nanceloid::note_off (int note) {
    if (note == this->note.note) {
        this->note.velocity = 0;
        this->note.start_time = clock;
    }
}

int Nanceloid::playing_note () {
    return note.velocity ? note.note : -1;
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

#ifdef DEBUG
            cout << "Received midi note off event: 0x" << hex << (int) note << endl;
#endif

        note_off (note);

    } else if (type == 0x90) {

        // handle note on events
        uint8_t note = data[1];
        uint8_t velocity = data[2];

#ifdef DEBUG
        cout << "Received midi note on event: 0x" << hex << (int) note << " 0x" << hex << (int) velocity << endl;
#endif

        note_on (note, velocity / 127.0);

    } else if (type == 0xe0) {

        // handle pitch bends
        uint8_t msb = data[2];

#ifdef DEBUG
        cout << "Received pitch bend event: 0x" << hex << (int) msb << endl;
#endif

        this->note.detune = msb / 127.0 * 4 - 2;

    } else if (type == 0xc0) {

        // handle program changes
        uint8_t id = data[1];

#ifdef DEBUG
        cout << "Received program change event: 0x" << hex << (int) id << endl;
#endif

        shape_i = id;
    }
}
