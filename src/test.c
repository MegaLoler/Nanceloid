#include <stdio.h>
#include <nnwnp.h>

int main(int argc, char **argv) {

    // create a test waveguide
    NN_Waveguide *wg = create_waveguide();

    // create a network that looks like this:
    // SOURCE <-> GUIDE <-> GUIDE <-> DRAIN
    NN_Node *node1 = spawn_node(wg, SOURCE);
    NN_Node *node2 = spawn_node(wg, GUIDE);
    NN_Node *node3 = spawn_node(wg, GUIDE);
    NN_Node *node4 = spawn_node(wg, DRAIN);
    NN_Link *link1 = link_nodes(node1, node2);
    NN_Link *link2 = link_nodes(node2, node3);
    NN_Link *link3 = link_nodes(node3, node4);
    inject_energy(node1, 1);

    // now run it for 1k samples
    for (int i = 0; i < 1000; i++) {
        run_waveguide(wg);

        // debug print
        printf("DEBUG:\n");
        printf("SOURCE: RIGHT=%.2f\n", link1->energy);
        printf("NODE 2: RIGHT=%.2f\n", link2->energy);
        printf("NODE 3: RIGHT=%.2f\n", link3->energy);
    }

    // destroy the waveguide and exit
    //destroy_waveguide(wg);
    return 0;
}
