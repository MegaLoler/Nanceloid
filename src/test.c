#include <stdio.h>
#include <math.h>
#include <waveguide.h>

int main (int argc, char **argv) {

    // create a test waveguide
    NN_Waveguide *wg = create_waveguide ();
    wg->damping = 0;

    // create a network that looks like this:
    // SOURCE <-> GUIDE <-> ... <-> GUIDE <-> DRAIN
    NN_Node *source = spawn_node (wg);
    NN_Node *node0 = spawn_node (wg);
    NN_Node *node1 = spawn_node (wg);
    NN_Node *node2 = spawn_node (wg);
    NN_Node *node3 = spawn_node (wg);
    NN_Node *node4 = spawn_node (wg);
    NN_Node *node5 = spawn_node (wg);
    NN_Node *node6 = spawn_node (wg);
    NN_Node *node7 = spawn_node (wg);
    NN_Node *node8 = spawn_node (wg);
    NN_Node *node9 = spawn_node (wg);
    NN_Node *drain = spawn_node (wg);
    NN_Link *link_source = double_link_nodes (source, node0);
    NN_Link *link0 = double_link_nodes (node0, node1);
    NN_Link *link1 = double_link_nodes (node1, node2);
    NN_Link *link2 = double_link_nodes (node2, node3);
    NN_Link *link3 = double_link_nodes (node3, node4);
    NN_Link *link4 = double_link_nodes (node4, node5);
    NN_Link *link5 = double_link_nodes (node5, node6);
    NN_Link *link6 = double_link_nodes (node6, node7);
    NN_Link *link7 = double_link_nodes (node7, node8);
    NN_Link *link8 = double_link_nodes (node8, node9);
    NN_Link *link9 = single_link_nodes (node9, drain);
    set_admittance (source, 0);
    set_admittance (drain, INFINITY);
    //set_admittance (node0, 0.5);
    //set_admittance (node5, 0.5);
    //set_admittance (node6, 2);
    //set_admittance (node7, 2);
    //set_admittance (node8, 2);
    //set_admittance (node9, 2);
    //destroy_node (node4);
    //destroy_link ((NN_Link *) list_get (node5->links, 0));
    //destroy_link (link4);

    add_energy (link_source, 1);
    // now run it for 1k samples
    for (int i = 0; i < 1000; i++) {
        run_waveguide (wg);

        //add_energy (link0, 1);

        // debug print
        printf ("DEBUG:\n");
        printf ("NODE S: RIGHT=%023.5f\n", link_source->energy);
        printf ("NODE 0: RIGHT=%023.5f\tLEFT=%023.5f\n", link0->energy, ((NN_Link *) list_get (node0->links, 0))->energy);
        printf ("NODE 1: RIGHT=%023.5f\tLEFT=%023.5f\n", link1->energy, ((NN_Link *) list_get (node1->links, 0))->energy);
        printf ("NODE 2: RIGHT=%023.5f\tLEFT=%023.5f\n", link2->energy, ((NN_Link *) list_get (node2->links, 0))->energy);
        printf ("NODE 3: RIGHT=%023.5f\tLEFT=%023.5f\n", link3->energy, ((NN_Link *) list_get (node3->links, 0))->energy);
        printf ("NODE 4: RIGHT=%023.5f\tLEFT=%023.5f\n", link4->energy, ((NN_Link *) list_get (node4->links, 0))->energy);
        printf ("NODE 5: RIGHT=%023.5f\tLEFT=%023.5f\n", link5->energy, ((NN_Link *) list_get (node5->links, 0))->energy);
        printf ("NODE 6: RIGHT=%023.5f\tLEFT=%023.5f\n", link6->energy, ((NN_Link *) list_get (node6->links, 0))->energy);
        printf ("NODE 7: RIGHT=%023.5f\tLEFT=%023.5f\n", link7->energy, ((NN_Link *) list_get (node7->links, 0))->energy);
        printf ("NODE 8: RIGHT=%023.5f\tLEFT=%023.5f\n", link8->energy, ((NN_Link *) list_get (node8->links, 0))->energy);
        printf ("NODE 9: RIGHT=%023.5f\tLEFT=%023.5f\n", link9->energy, ((NN_Link *) list_get (node9->links, 0))->energy);
        printf ("NET:   %f\n", net_waveguide_energy (wg));
        printf ("DRAIN: %f\n", collect_drain (wg));

    }

    // destroy the waveguide and exit
    destroy_waveguide (wg);
    return 0;
}
