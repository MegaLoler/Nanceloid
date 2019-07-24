#pragma once

#include <segment.h>

class Waveguide {
    private:
        // the segments
        Segment *segments;
        int length;

        // physical parameters
        double damping;
        double turbulence;

        // drain collectors for energy lost from left and right side
        double drain_left = 0;
        double drain_right = 0;

        // impedance on the left and right openings
        double left_opening_impedance;
        double right_opening_impedance;

        // calculate a reflection coefficient given source and target impedance
        static double calculate_gamma (double source_impedance, double target_impedance);

        // generate normalized noise between -1 and 1
        static double noise ();

    public:
        Waveguide (int length, double damping = 0.04, double turbulence = 0.1,
                   double left_opening_impedance = INFINITY,
                   double right_opening_impedance = 0.1);
        ~Waveguide ();

        double get (int i);         // get sum energy at a given segment
        double get_left (int i);    // get left going energy at a segment
        double get_right (int i);   // get right going energy at a segment
        double get_net ();          // get sum of energy in all segments

        // collect the energy drain on left and right sides
        double collect_drain_left ();   // get the loss from the left end
        double collect_drain_right ();  // get the loss from the right end

        // put energy into a segment
        void put (int i, double left, double right);

        // calculate reflection coefficients
        void prepare ();

        // make new changes active
        void flush ();

        // run the waveguide for one frame
        void run ();

        // print the state of the waveguide for debugging
        void debug ();

        // get the number of segments
        int get_length ();

        // get a segment from the waveguide
        Segment &get_segment (int i);
};
