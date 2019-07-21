#ifndef WAVEGUIDE_H
#define WAVEGUIDE_H

#include <list.h>
#include <stdint.h>

// defaults
#define DAMPING 0//0.04
#define TURBULENCE 0//0.001



#ifdef __cplusplus
extern "C" {
#endif



//////// "HOT" DATA TYPES //////////

typedef struct NN_Link NN_Link;

// represents a waveguide node
typedef struct NN_Node {
    double area;        // cross sectional area at node region (related to admittance and impedance)
    List *links;        // list of links to other nodes from here
} NN_Node;

// create a new node of a given type and with a given area
NN_Node *create_node (double area);

// destroy a node
void destroy_node (NN_Node *node);

// return the total energy in all the links of a node
double net_node_energy (NN_Node *node);

// get or set the impedance or admittance values of a node
double get_impedance (NN_Node *node);
double get_admittance (NN_Node *node);
void set_impedance (NN_Node *node, double Z);
void set_admittance (NN_Node *node, double Y);

// represents a single link from a node to another node
typedef struct NN_Link {
    NN_Node *source;    // the linked source node
    NN_Node *target;    // the linked target node
    double energy;      // the amount of energy traveling in this direction
    double queued;      // the amount of energy queued up for this node next simulation iteration
} NN_Link;

// creates a new link
NN_Link *create_link (NN_Node *source, NN_Node *target);

// destroys a link
void destroy_link (NN_Link *link);

// adds energy to a link (adds it to the queue)
void add_energy (NN_Link *link, double energy);

// represents a waveguide network
typedef struct NN_Waveguide {
    List *nodes;        // list of waveguide nodes
    double damping;     // reflection loss coefficient 
    double turbulence;  // turbulence amplitude coefficient
    double drain;       // lost acoustic energy
} NN_Waveguide;

// create a new waveguide network object
NN_Waveguide *create_waveguide ();

// destroy a waveguide network object
void destroy_waveguide (NN_Waveguide *waveguide);

// run the waveguide simulation for a single time unit
void run_waveguide (NN_Waveguide *waveguide);

// distribute energy to all the links in a node
void inject_energy (NN_Waveguide *waveguide, NN_Node *node, double energy);

// return the total energy in the entire waveguide network
double net_waveguide_energy (NN_Waveguide *waveguide);

// return the drain and then clear it
double collect_drain (NN_Waveguide *waveguide);

// create a new node and add it to a waveguide automatically
// uses a default area of 1
NN_Node *spawn_node (NN_Waveguide *waveguide);

// singly link source node to target node
NN_Link *single_link_nodes (NN_Node *source, NN_Node *target);

// mutually link source and target nodes
NN_Link *double_link_nodes (NN_Node *source, NN_Node *target);

// link a node with itself
NN_Link *terminate_node (NN_Node *node);



//////// "COLD" DATA TYPES ////////////////

// TODO: binary friendly versions of the above structs

// serialize a waveguide into a binary format suitable for saving to disk
uint8_t *serialize (NN_Waveguide *waveguide);

// deserialize a previously serialized waveguide
NN_Waveguide *deserialize (uint8_t *data);



#ifdef __cplusplus
}
#endif



#endif
