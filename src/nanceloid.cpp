#include <nanceloid.h>
#include <iostream>
#include <cmath>

using namespace std;

void Nanceloid::set_rate (int rate) {
    if (rate != this->rate) {
        this->rate = rate;
        init ();
    }
}

void Nanceloid::run (float *out) {
    // run control rate operations
    if (clock++ % control_rate_divider == 0)
        run_control ();

    // TEMP: sine osc for testing
    float osc = sin (osc_phase * M_PI * 2) * pressure;
    osc_phase += frequency / rate;

    // mix and return the samples
    float sample = osc * params.volume.value;                   // output volume
    float pan = params.panning.get_normalized_value () / 2;     // panning
    out[0] = cos (pan * M_PI) * sample;
    out[1] = sin (pan * M_PI) * sample;
}

void Nanceloid::run_control () {
    // run tremolo lfo
    tremolo_osc = sin (tremolo_phase * M_PI * 2) * params.tremolo_depth.value;
    tremolo_phase += params.tremolo_rate.value / rate;

    // run vibrato lfo
    vibrato_osc = sin (vibrato_phase * M_PI * 2) * params.vibrato_depth.value;
    vibrato_phase += params.vibrato_rate.value / rate;

    // run adsr envelope to get current input pressure
    float delta_clock = clock - note.start_time;  // samples since note event
    float delta_time = delta_clock / rate;        // seconds since note event
    float sustain = params.adsr_sustain.value * note.velocity
        * (1 - params.min_velocity.value) + params.min_velocity.value;  // effective sustain level
    if (note.velocity) {
        // note is on
        if (delta_time < params.adsr_attack.value)
            // attack
            pressure = delta_time / params.adsr_attack.value;
        else if (delta_time < params.adsr_attack.value + params.adsr_decay.value)
            // decay
            pressure = 1 - (delta_time - params.adsr_attack.value) / params.adsr_decay.value;
        else
            // sustain
            pressure = sustain;
    } else {
        // note is off
        if (delta_time < params.adsr_attack.value)
            // release
            pressure = sustain - sustain * delta_time / params.adsr_release.value;
        else
            // post-release
            pressure = 0;
    }
}

void Nanceloid::init () {

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
