// include file for the negative nancy's waveguide network playground C library

#include <list.h>
#include <stdint.h>

// defaults
#define DAMPING 0.04
#define TURBULENCE 0.001



#ifdef __cplusplus
extern "C" {
#endif



//////// "HOT" DATA TYPES //////////

typedef struct NN_Link NN_Link;

// representns a type of waveguide node
typedef enum NN_NodeType {
    GUIDE,      // a normal node that guides energy
    SOURCE,     // a node that emits energy (input node)
    DRAIN       // a node that absorbs energy (output node)
} NN_NodeType;

// represents a waveguide node
typedef struct NN_Node {
    NN_NodeType type;   // the type of node it is
    double area;        // cross sectional area at node region (related to admittance and impedance)
    List *links;        // list of links to other nodes from here
} NN_Node;

// create a new node of a given type and with a given area
NN_Node *create_node (NN_NodeType type, double area);

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

// create a new node and add it to a waveguide automatically
// uses a default area of 1
NN_Node *spawn_node (NN_Waveguide *waveguide, NN_NodeType type);

// link two nodes mutually
// returns the link from a to b
NN_Link *link_nodes (NN_Node *a, NN_Node *b);



//////// "COLD" DATA TYPES ////////////////

// TODO: binary friendly versions of the above structs

// serialize a waveguide into a binary format suitable for saving to disk
uint8_t *serialize (NN_Waveguide *waveguide);

// deserialize a previously serialized waveguide
NN_Waveguide *deserialize (uint8_t *data);



#ifdef __cplusplus
}
#endif
