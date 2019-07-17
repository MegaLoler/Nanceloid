#include <nnwnp.h>

int main(int argc, char **argv) {
    NN_Waveguide *waveguide = create_waveguide();
    destroy_waveguide(waveguide);
    return 0;
}
