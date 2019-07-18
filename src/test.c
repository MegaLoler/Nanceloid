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
    NN_Node *node4 = spawn_node(wg, GUIDE);
    NN_Node *node5 = spawn_node(wg, DRAIN);
    NN_Link *link1 = link_nodes(node1, node2);
    NN_Link *link2 = link_nodes(node2, node3);
    NN_Link *link3 = link_nodes(node3, node4);
    NN_Link *link4 = link_nodes(node4, node5);
    inject_energy(wg, node1, 1);

    // now run it for 1k samples
    for (int i = 0; i < 1000; i++) {
        run_waveguide(wg);

        // debug print
        printf("DEBUG:\n");
        printf("SOURCE: RIGHT=%.5f\n",            link1->energy);
        printf("NODE 2: RIGHT=%.5f\tLEFT=%.5f\n", link2->energy, node2->links[0]->energy);
        printf("NODE 3: RIGHT=%.5f\tLEFT=%.5f\n", link3->energy, node3->links[0]->energy);
        printf("NODE 4: RIGHT=%.5f\tLEFT=%.5f\n", link4->energy, node4->links[0]->energy);
        printf("DRAIN:            \tLEFT=%.5f\n",                node5->links[0]->energy);

    }

    // destroy the waveguide and exit
    //destroy_waveguide(wg);
    return 0;
}
