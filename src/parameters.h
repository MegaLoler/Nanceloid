#pragma once

#include <iostream>
#include <iomanip>
#include <cstdint>

// represents a live parameter
struct Parameter {
    const char *name;
    const char *short_name;
    const char *label;
    float min;
    float max;
    float display_min;
    float display_max;
    float value;
    uint64_t cc_map;    // which midi controllers control this parameter

    Parameter (const char *name, const char *short_name, const char *label,
               float min, float max, float display_min, float display_max,
               float value, uint64_t cc_map = 0)
        : name (name), short_name (short_name), label (label),
          min (min), max (max), display_min (display_min), display_max (display_max),
          value (value), cc_map (cc_map) {}

    // sets value given normalized value
    void set_normalized_value (float value) {
        this->value = value * (max - min) + min;
    }

    // return values in range 0 to 1
    float get_normalized_value () {
        return (value - min) / (max - min);
    }

    // sets the value given a midi value 0 to 127
    void set_midi_value (int value) {
        set_normalized_value (value / 127.0);
    }

    float get_display_value () {
        return get_normalized_value () * (display_max - display_min) + display_min;
    }

    // map a midi controller to this parameter
    void map_cc (int cc) {
        cc_map |= 1 << cc;
    }

    // unmap a midi controller from this parameter
    void unmap_cc (int cc) {
        cc_map &= ~(1 << cc);
    }

    // whether this parameter is mapped to a given midi cc
    bool is_mapped (int cc) {
        return cc_map & (1 << cc);
    }
};

// represents live synth parameters
struct Parameters {

    // physical parameters
    Parameter tract_length    = Parameter ("Tract Length",     "TractLen", "cm",    8,    24,   8,   24,    17.5);
    Parameter refl_left       = Parameter ("Left Reflection",  "Left Rfl", "%",     0,    1,    0,   100,   0.75);
    Parameter refl_right      = Parameter ("Right Reflection", "RightRfl", "%",    -0.5, -1,   -50, -100,  -0.9);
    Parameter nose_admittance = Parameter ("Nose Admittance",  "NoseAdmt", "",      0,    1,    0,   1,     0.25);
    Parameter crossfade       = Parameter ("Crossfade Tract",  "Crossfad", "%",     0,    1,    0,   100,   0.2);

    // musical and audio parameters
    Parameter tremolo_rate    = Parameter ("Tremolo Rate",     "Trm.Rate", "hz",    1,    16,   1,   16,    5);
    Parameter tremolo_depth   = Parameter ("Tremolo Depth",    "Trm.Dept", "%",     0,    1,    0,   100,   0.25);
    Parameter vibrato_rate    = Parameter ("Vibrato Rate",     "Vib.Rate", "hz",    1,    16,   1,   16,    5);
    Parameter vibrato_depth   = Parameter ("Vibrato Depth",    "Vib.Dept", "cents", 0,    2,    0,   200,   0.25);
    Parameter adsr_attack     = Parameter ("ADSR Attack",      "Attack",   "ms",    0,    1,    0,   1000,  0.1);
    Parameter adsr_decay      = Parameter ("ADSR Decay",       "Decay",    "ms",    0,    1,    0,   1000,  0.1);
    Parameter adsr_sustain    = Parameter ("ADSR Sustain",     "Sustain",  "%",     0,    1,    0,   100,   1);
    Parameter adsr_release    = Parameter ("ADSR Release",     "Release",  "ms",    0,    2,    0,   2000,  0.1);
    Parameter min_velocity    = Parameter ("Minimum Velocity", "Min. Vel", "%",     0,    1,    0,   100,   0.5);
    Parameter panning         = Parameter ("Panning",          "Panning",  "%",    -1,    1,   -100, 100,   0);
    Parameter volume          = Parameter ("Volume",           "Volume",   "%",     0,    1,    0,   100,   0.25);

    // return an array of the parameters
    Parameter *as_array () {
        return (Parameter *) this;
    }

    // return the length of that array
    inline int length () {
        return sizeof (Parameters) / sizeof (Parameter);
    }

    // display the current values
    void print () {
        std::cout << "[parameters]\n";
        std::cout << "          [name] [short]        [display] [value] [normalized]\n";
        Parameter *array = as_array ();
        for (int i = 0; i < length (); i++) {
            Parameter &p = array[i];
            std::cout           << std::setw (16) << std::right << p.name
                      << " ("   << std::setw (8)  << std::left  << p.short_name
                      << ") = " << std::setw (5)  << std::right << p.get_display_value ()
                      << " "    << std::setw (5)  << std::left  << p.label
                      << " ; "  << std::setw (5)                << p.value
                      << " ; "                                  << p.get_normalized_value ()
                      << "\n";
        }
        std::cout << std::flush;
    }
};
