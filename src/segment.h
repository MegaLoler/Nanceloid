#pragma once

class Segment {
    private:
        double left = 0;    // left traveling wave energy
        double right = 0;   // right traveling wave energy
        double area;        // cross sectional area

        // these values are moved to the above after flush is called
        double _left = 0;   // pending left value
        double _right = 0;  // pending left value

        // precalculated reflection coefficients
        double gamma_left;
        double gamma_right;

    public:
        Segment (double area = 1)
            : area (area) {}
        ~Segment () {}

        // get or set the acoustic admittance or impedance
        double get_admittance ();
        double get_impedance ();
        void set_admittance (double admittance);
        void set_impedance (double impedance);

        double get ();          // get sum of the traveling waves
        double get_left ();     // get energy traveling left
        double get_right ();    // get energy traveling right

        // add left and right going acoustic energy
        void put (double left, double right);

        // put the pending changes in energy into action!!!!!!!!!
        void flush ();

        // get and set the reflection coefficients
        void set_gamma (double left, double right);
        double get_gamma_left ();
        double get_gamma_right ();

        // copy acoustic energy only from another segment
        void copy (Segment &source);
};
