#include <stdio.h>
#include <waveguide.h>

int main (int argc, char **argv) {

    // create a test waveguide
    NN_Waveguide *wg = create_waveguide ();

    // create a network that looks like this:
    // SOURCE <-> GUIDE <-> ... <-> GUIDE <-> DRAIN
    NN_Node *node0 = spawn_node (wg, SOURCE);
    NN_Node *node1 = spawn_node (wg, GUIDE);
    NN_Node *node2 = spawn_node (wg, GUIDE);
    NN_Node *node3 = spawn_node (wg, GUIDE);
    NN_Node *node4 = spawn_node (wg, GUIDE);
    NN_Node *node5 = spawn_node (wg, GUIDE);
    NN_Node *node6 = spawn_node (wg, GUIDE);
    NN_Node *node7 = spawn_node (wg, GUIDE);
    NN_Node *node8 = spawn_node (wg, GUIDE);
    NN_Node *node9 = spawn_node (wg, DRAIN);
    //NN_Node *node9 = spawn_node (wg, SOURCE);
    NN_Link *link0 = link_nodes (node0, node1);
    NN_Link *link1 = link_nodes (node1, node2);
    NN_Link *link2 = link_nodes (node2, node3);
    NN_Link *link3 = link_nodes (node3, node4);
    NN_Link *link4 = link_nodes (node4, node5);
    NN_Link *link5 = link_nodes (node5, node6);
    NN_Link *link6 = link_nodes (node6, node7);
    NN_Link *link7 = link_nodes (node7, node8);
    NN_Link *link8 = link_nodes (node8, node9);
    inject_energy (wg, node0, 1);
    //set_admittance (node5, 1.1);
    //set_admittance (node6, 1.1);
    //set_admittance (node7, 1.1);
    //set_admittance (node8, 1.1);
    //destroy_node (node4);
    //destroy_link ((NN_Link *) list_get (node5->links, 0));
    //destroy_link (link4);

    // now run it for 1k samples
    for (int i = 0; i < 1000; i++) {
        run_waveguide (wg);

        // debug print
        printf ("DEBUG:\n");
        printf ("SOURCE: RIGHT=%023.5f\n",            link0->energy);
        printf ("NODE 1: RIGHT=%023.5f\tLEFT=%023.5f\n", link1->energy, ((NN_Link *) list_get (node1->links, 0))->energy);
        printf ("NODE 2: RIGHT=%023.5f\tLEFT=%023.5f\n", link2->energy, ((NN_Link *) list_get (node2->links, 0))->energy);
        printf ("NODE 3: RIGHT=%023.5f\tLEFT=%023.5f\n", link3->energy, ((NN_Link *) list_get (node3->links, 0))->energy);
        printf ("NODE 4: RIGHT=%023.5f\tLEFT=%023.5f\n", link4->energy, ((NN_Link *) list_get (node4->links, 0))->energy);
        printf ("NODE 5: RIGHT=%023.5f\tLEFT=%023.5f\n", link5->energy, ((NN_Link *) list_get (node5->links, 0))->energy);
        printf ("NODE 6: RIGHT=%023.5f\tLEFT=%023.5f\n", link6->energy, ((NN_Link *) list_get (node6->links, 0))->energy);
        printf ("NODE 7: RIGHT=%023.5f\tLEFT=%023.5f\n", link7->energy, ((NN_Link *) list_get (node7->links, 0))->energy);
        printf ("NODE 8: RIGHT=%023.5f\tLEFT=%023.5f\n", link8->energy, ((NN_Link *) list_get (node8->links, 0))->energy);
        printf ("DRAIN:                            \tLEFT=%023.5f\n",                ((NN_Link *) list_get (node9->links, 0))->energy);
        printf ("NET: %f\n", net_waveguide_energy (wg));

    }

    // destroy the waveguide and exit
    //destroy_waveguide (wg);
    return 0;
}
