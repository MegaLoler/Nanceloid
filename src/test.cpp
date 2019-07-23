#include <iostream>
#include <cmath>
#include <waveguide.h>

using namespace std;

// how many samples to run the waveguide for
const int samples = 1000;

int main (int argc, char **argv) {

    // create a test waveguide
    // length = 10 segments
    // damping = 0
    // turbulence = 0
    // left impedance = inf
    // right impedance = 0
    Waveguide wg (10, 0, 0, INFINITY, 0);

    // prepare the waveguide for use
    // (calculate the reflection coefficients)
    wg.prepare ();

    // add an impluse of 1 going right at the first segment
    wg.put (0, 0, 1);

    // now run it for some amount of samples
    for (int i = 0; i < samples; i++) {

        // debug print
        cout << "Test iteration #" << i + 1 << endl;
        wg.debug ();

        // run it for this iteration
        wg.run ();

        // apply continuous pressure
        //wg.put (0, 0, 1);

    }

    return 0;
}
