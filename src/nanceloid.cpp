#include <iostream>
#include <cmath>
#include <nanceloid.h>

using namespace std;

Nanceloid::~Nanceloid () {
    // NOTE: is this necessary?
    if (waveguide != nullptr)
        delete waveguide;
}

void Nanceloid::debug () {
    Parameters &p = parameters;

    cout << "[DEBUG PARAMETERS] " << endl;
    cout << "lungs              " << p.lungs << endl;
    cout << "glottal_tension    " << p.glottal_tension << endl;
    cout << "laryngeal_height   " << p.laryngeal_height << endl;
    cout << "lips_roundedness   " << p.lips_roundedness << endl;
    cout << "jaw_height         " << p.jaw_height << endl;
    cout << "tongue_frontness   " << p.tongue_frontness << endl;
    cout << "tongue_height      " << p.tongue_height << endl;
    cout << "tongue_flatness    " << p.tongue_flatness << endl;
    cout << "velic_closure      " << p.velic_closure << endl;
    cout << "acoustic_damping   " << p.acoustic_damping << endl;
    cout << "enunciation        " << p.enunciation << endl;
    cout << "portamento         " << p.portamento << endl;
    cout << "frication          " << p.frication << endl;
    cout << "surface_tension    " << p.surface_tension << endl;
    cout << "tract_length       " << p.tract_length << endl;
    cout << "ambient_admittance " << p.ambient_admittance << endl;
    cout << "vibrato_rate       " << p.vibrato_rate << endl;
    cout << "vibrato_depth      " << p.vibrato_depth << endl;
    cout << "velocity           " << p.velocity << endl;
    cout << "volume             " << p.volume << endl;;
    cout << endl;
}

void Nanceloid::set_rate (int rate) {
    if (rate != this->rate) {
        this->rate = rate;
        init ();
    }
}

double Nanceloid::run () {

    // air pressure from lungs
    parameters.lungs += (note.velocity - parameters.lungs) * parameters.portamento * portamento;
    double opening = fmax (-parameters.glottal_tension, 0);
    waveguide->put (0, 0, parameters.lungs * opening);

    // vocal cord vibration
    waveguide->put (0, 0, source->run (this));

    // handle articulatory dynamics
    // TODO: make this control rate
    reshape ();

    // simulate acoustics
    waveguide->run ();

    // return the drain output
    return waveguide->collect_drain_right () * parameters.volume;
}

void Nanceloid::approach_admittance (Segment &segment, double target, double coefficient) {
    double current = segment.get_admittance ();
    double delta = (target - current) * coefficient;
    double new_admittance = current + delta;
    segment.set_admittance (new_admittance);
}

void Nanceloid::reshape (bool set) {

    double coefficient = set ? 1 : parameters.enunciation * enunciation;

    // shape the lips
    double lips_admittance = (1 - parameters.lips_roundedness) / neutral_impedance;
    for (int i = lips_start; i < waveguide->get_length (); i++) {
        Segment &segment = waveguide->get_segment (i);
        approach_admittance (segment, lips_admittance, coefficient);
    }

    // shape the tongue
    for (int i = tongue_start; i < lips_start; i++) {
        Segment &segment = waveguide->get_segment (i);

        // TEMP
        // TODO: enhance this lol
        double unit_pos = i / (double) ((lips_start - tongue_start) - 1);
        double phase = unit_pos - parameters.tongue_frontness;
        double value = cos (phase * M_PI / 2) * parameters.tongue_height;
        double unit_area = 1 - value;
        approach_admittance (segment, unit_area / neutral_impedance, coefficient);
    }

    //// shape the velum TODO
    //double velum_admittance = (1 - voice->parameters.velic_closure) / NASAL_Z;
    //set_admittance (voice->nose[0], velum_admittance);

    waveguide->prepare ();
}

void Nanceloid::init () {

    Waveguide *old = waveguide;

    // calculate the physical length of a single segment (cm)
    double unit = speed_of_sound / rate;

    // generate enough segments to meet the desired length
    int length = ceil (parameters.tract_length / unit);

    // create the new waveguide
    waveguide = new Waveguide (length, parameters.acoustic_damping, parameters.frication,
            INFINITY, 1 / parameters.ambient_admittance);

    // calculate the positions of various landmarks along the tract
    // TODO: calculate proportions based on overal tract length
    larynx_start = floor (length * 0);
    tongue_start = floor (length * 0.2);
    lips_start   = floor (length * 0.9);

    // if there was a previous waveguide, then copy the old sound state to the new one
    // and of course delete the old one
    if (old != nullptr) {
        waveguide->copy (old);

        // TODO: why does this break lol
        //delete old;
    }

    reshape (true);
}

double Nanceloid::map_to_range (uint8_t value, double min, double max) {
    return value / 127.0 * (max - min) + min;
}

double Nanceloid::map_velocity (uint8_t value) {
    double normal = map_to_range (value, 0, 1);
    return (1 - parameters.velocity) + parameters.velocity * normal;
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

        switch (id) {
            case controller_vibrato:
                this->parameters.vibrato_depth = map_to_range (value, 0, 1);
                break;

            case controller_glottal_tension:
                this->parameters.glottal_tension = map_to_range (value, -1, 1);
                break;

            case controller_lips_roundedness:
                this->parameters.lips_roundedness = map_to_range (value, 0, 1);
                break;

            case controller_jaw_height:
                this->parameters.jaw_height = map_to_range (value, 0, 1);
                break;

            case controller_tongue_frontness:
                this->parameters.tongue_frontness = map_to_range (value, 0, 1);
                break;

            case controller_tongue_height:
                this->parameters.tongue_height = map_to_range (value, 0, 1);
                break;

            case controller_tongue_flatness:
                this->parameters.tongue_flatness = map_to_range (value, -1, 1);
                break;

            case controller_enunciation:
                this->parameters.enunciation = map_to_range (value, 0, 1);
                break;

            case controller_tract_length:
                this->parameters.tract_length = map_to_range (value, 8, 24);
                init ();
                break;
        }

#ifdef DEBUG
        debug ();
#endif

    } else if (type == 0x80) {

        // handle note off events
        uint8_t note = data[1];
        if (note == this->note.note)
            this->note.velocity = 0;

    } else if (type == 0x90) {

        // handle note on events
        uint8_t note = data[1];
        uint8_t velocity = data[2];

        this->note.note = note;
        this->note.velocity = map_velocity (velocity);

    } else if (type == 0xe0) {

        // handle pitch bends
        
        // get the least and most significant bits
        //uint8_t lsb = data[1];
        uint8_t msb = data[2];
        //int value = (lsb + msb) << 7;

#ifdef DEBUG
        cout << "Received pitch bend: 0x" << hex << (int) msb << endl;
#endif

        this->note.detune = map_to_range (msb, -2, 2);

    }

    // TODO: handle phoneme mapped notes on channel 10 or w/e
}

Parameters &Nanceloid::get_parameters () {
    return parameters;
}
