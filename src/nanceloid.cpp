#include <iostream>
#include <cmath>
#include <nanceloid.h>

using namespace std;

Nanceloid::~Nanceloid () {
    if (throat != nullptr)
        delete throat;
    if (mouth != nullptr)
        delete mouth;
    if (nose != nullptr)
        delete nose;
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
    cout << "panning            " << p.panning << endl;;
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
    throat->put (0, 0, parameters.lungs * opening);

    // vocal cord vibration
    throat->put (0, 0, source->run (this));

    // DEBUG:
    //throat->put (0, 0, 1);

    // handle articulatory dynamics
    // TODO: make this control rate
    reshape ();

    // simulate acoustics
    throat->run ();
    mouth->run ();
    nose->run ();

    // handle the throat-mouth-nose junction
    double throat_drain = throat->collect_drain_right ();
    double mouth_drain = mouth->collect_drain_left ();
    double nose_drain = nose->collect_drain_left ();

    Segment &junction_throat = throat->get_segment (nose_start - 1);
    Segment &junction_mouth = mouth->get_segment (0);
    Segment &junction_nose = nose->get_segment (0);

    double throat_admittance = junction_throat.get_admittance ();
    double mouth_admittance = junction_mouth.get_admittance ();
    double nose_admittance = junction_nose.get_admittance ();

    double throat_mouth_admittance = throat_admittance + mouth_admittance;
    double throat_nose_admittance = throat_admittance + nose_admittance;
    double mouth_nose_admittance = mouth_admittance + nose_admittance;
    
    double throat_to_mouth_weight = mouth_admittance / mouth_nose_admittance;
    double throat_to_nose_weight = nose_admittance / mouth_nose_admittance;
    double mouth_to_throat_weight = throat_admittance / throat_nose_admittance;
    double mouth_to_nose_weight = nose_admittance / throat_nose_admittance;
    double nose_to_throat_weight = throat_admittance / throat_mouth_admittance;
    double nose_to_mouth_weight = mouth_admittance / throat_mouth_admittance;

    junction_throat.put_direct (mouth_drain * mouth_to_throat_weight, 0);
    junction_throat.put_direct (nose_drain * nose_to_throat_weight, 0);
    junction_mouth.put_direct (0, throat_drain * throat_to_mouth_weight);
    junction_mouth.put_direct (0, nose_drain * nose_to_mouth_weight);
    junction_nose.put_direct (0, throat_drain * throat_to_nose_weight);
    junction_nose.put_direct (0, mouth_drain * mouth_to_nose_weight);
    //junction_throat.put_direct (mouth_drain, 0);
    //junction_mouth.put_direct (0, throat_drain);

    // return the drain output
    double drain = mouth->collect_drain_right () + nose->collect_drain_right ();
    return drain * parameters.volume;
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
    for (int i = lips_start; i < length; i++) {
        Segment &segment = get_throat_mouth_segment (i);
        approach_admittance (segment, lips_admittance, coefficient);
    }

    // shape the tongue
    for (int i = tongue_start; i < lips_start; i++) {
        Segment &segment = get_throat_mouth_segment (i);

        // TEMP
        // TODO: enhance this lol
        double unit_pos = i / (double) ((lips_start - tongue_start) - 1);
        double phase = unit_pos - parameters.tongue_frontness;
        double value = cos (phase * M_PI / 2) * parameters.tongue_height;
        double unit_area = 1 - value;
        //approach_admittance (segment, unit_area / neutral_impedance * 2, coefficient);
        approach_admittance (segment, unit_area / neutral_impedance, coefficient);
    }

    // shape the velum
    double velum_admittance = (1 - parameters.velic_closure) / neutral_impedance;
    approach_admittance (nose->get_segment (0), velum_admittance, coefficient);

    // set junction output impedances
    Segment &junction_throat = throat->get_segment (nose_start - 1);
    Segment &junction_mouth = mouth->get_segment (0);
    Segment &junction_nose = nose->get_segment (0);
    double throat_admittance = junction_throat.get_admittance ();
    double mouth_admittance = junction_mouth.get_admittance ();
    double nose_admittance = junction_nose.get_admittance ();
    double throat_mouth_admittance = throat_admittance + mouth_admittance;
    double throat_nose_admittance = throat_admittance + nose_admittance;
    double mouth_nose_admittance = mouth_admittance + nose_admittance;
    throat->set_right_opening_impedance(1 / mouth_nose_admittance);
    mouth->set_left_opening_impedance(1 / throat_nose_admittance);
    nose->set_left_opening_impedance(1 / throat_mouth_admittance);
    throat->set_right_opening_impedance(1 / mouth_admittance);
    mouth->set_left_opening_impedance(1 / throat_admittance);

    // precalculate reflection coefficients
    throat->prepare ();
    mouth->prepare ();
    nose->prepare ();
}

Segment &Nanceloid::get_throat_mouth_segment (int i) {
    if (i < nose_start)
        return throat->get_segment (i);
    else
        return mouth->get_segment (i - nose_start);
}

void Nanceloid::init () {

    Waveguide *old_throat = throat;
    Waveguide *old_mouth = mouth;
    Waveguide *old_nose = nose;

    // calculate the physical length of a single segment (cm)
    double unit = speed_of_sound / rate;

    // generate enough segments to meet the desired length
    length = ceil (parameters.tract_length / unit);

    // calculate the positions of various landmarks along the tract
    // TODO: calculate proportions based on overal tract length
    larynx_start = floor (length * 0);
    tongue_start = floor (length * 0.2);
    nose_start   = floor (length * 0.3);
    lips_start   = floor (length * 0.95);

    // create the throat
    throat = new Waveguide (nose_start, parameters.acoustic_damping, parameters.frication,
            INFINITY, 0);

    // create the mouth
    mouth = new Waveguide (length - nose_start, parameters.acoustic_damping, parameters.frication,
            0, 1 / parameters.ambient_admittance);

    // create the nose
    nose = new Waveguide (length - nose_start, parameters.acoustic_damping,
            parameters.frication, 0, 1 / parameters.ambient_admittance);

    // shape the larynx
    for (int i = 0; i < tongue_start; i++) {
        Segment &segment = get_throat_mouth_segment (i);
        segment.set_admittance (0.2);
    }

    // if there was a previous waveguide, then copy the old sound state to the new one
    // and of course delete the old one
    if (old_throat != nullptr) {
        throat->copy (old_throat);
        // TODO: why does this break lol
        // delete old_throat
    }
    if (old_mouth != nullptr) {
        mouth->copy (old_mouth);
        // TODO: why does this break lol
        // delete old_mouth
    }
    if (old_nose != nullptr) {
        nose->copy (old_nose);
        // TODO: why does this break lol
        // delete old_nose
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

            case controller_velum:
                this->parameters.velic_closure = map_to_range (value, 0, 1);
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
