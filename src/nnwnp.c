#include <nnwnp.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

// util function to dynamically append to an array
// given pointer to the array, current lengeth, size of individual elements, and new element to add
// returns new pointer
// TODO: rework this, gonna need to have POINTERS TO POINTERS... i guessss!!!
void *append(void *array, int *length, size_t size, void *new) {
    void *new_array = realloc(array, size * ((*length) + 1));
    memcpy(new_array + (*length) * size, new, size);
    (*length)++;
    return new_array;
}

NN_Node *create_node(NN_NodeType type, double area) {
    NN_Node *node = (NN_Node *)malloc(sizeof(NN_Node));
    node->type = type;
    node->area = area;
    node->links = malloc(0);
    node->num_links = 0;
    return node;
}

void destroy_node(NN_Node *node) {
    for (int i = 0; i < node->num_links; i++) {
        destroy_link(&(node->links[i]));
    }
    free(node);
}

double area_to_admittance(double area) {
    return area;
}

double admittance_to_area(double Y) {
    return Y;
}

double get_impedance(NN_Node *node) {
    switch (node->type) {
        case SOURCE:
            return INFINITY;
        case DRAIN:
            return 0;
        case GUIDE:
        default:
            return 1 / get_admittance(node);
    }
}

double get_admittance(NN_Node *node) {
    switch (node->type) {
        case SOURCE:
            return 0;
        case DRAIN:
            return INFINITY;
        case GUIDE:
        default:
            return area_to_admittance(node->area);
    }
}

void set_impedance(NN_Node *node, double Z) {
    set_admittance(node, 1 / Z);
}

void set_admittance(NN_Node *node, double Y) {
    node->area = admittance_to_area(Y);
}

NN_Link *create_link(NN_Node *source, NN_Node *target) {
    NN_Link *link = (NN_Link *)malloc(sizeof(NN_Link));
    link->source = source;
    link->target = target;
    link->energy = 0;
    link->queued = 0;
    return link;
}

void destroy_link(NN_Link *link) {
    // TODO: also destroy accompanying link and remove self from source and targets list of links
    free(link);
}

void add_energy(NN_Link *link, double energy) {
    link->queued += energy;
}

void flush_link(NN_Link *link) {
    link->energy = link->queued;
    link->queued = 0;
}

double reflection(double source_Y, double target_Y) {
    if (source_Y == target_Y)
        return 0;
    else if (source_Y == 0 || target_Y == INFINITY)
        return -1;
    else if (source_Y == INFINITY || target_Y == 0)
        return 1;
    else {
        double source_Z = 1 / source_Y;
        double target_Z = 1 / target_Y;
        return (target_Z - source_Z) / (target_Z + source_Z);
    }
}

// source = originating node, OR NULL if none
void distribute_energy(NN_Node *node, double energy, NN_Node *source) {
    // calculate the net admittance away from the target junction
    double admittance = 0;

    // the link to store reflection energy
    NN_Link *reflection_link = NULL;

    for (int i = 0; i < node->num_links; i++) {
        NN_Link *jlink = &(node->links[i]);
        NN_Node *jtarget = jlink->target;

        if (jtarget != source) {
            admittance += get_admittance(jtarget);
        } else {
            reflection_link = jlink;
        }
    }

    if (reflection_link) {
        // get the reflected energy
        double gamma = reflection(get_admittance(node), admittance);
        double reflection = gamma * energy;

        // send the reflected energy backward
        add_energy(reflection_link, reflection);

        // subtract from remaining energy to be distributed
        energy -= reflection;
    }

    // distribute the remainder of the energy
    for (int i = 0; i < node->num_links; i++) {
        NN_Link *jlink = &(node->links[i]);

        if (jlink != reflection_link) {
            // get the weight for this link
            double weight = get_admittance(jlink->target) / admittance;

            // move the weighted amonut of energy
            add_energy(jlink, weight * energy);
        }
    }
}

void move_energy(NN_Link *link) {
    distribute_energy(link->target, link->energy, link->source);
}

void inject_energy(NN_Node *node, double energy) {
    distribute_energy(node, energy, NULL);
}

NN_Waveguide *create_waveguide() {
    NN_Waveguide *waveguide = (NN_Waveguide *)malloc(sizeof(NN_Waveguide));
    waveguide->nodes = malloc(0);
    waveguide->num_nodes = 0;
    return waveguide;
}

void destroy_waveguide(NN_Waveguide *waveguide) {
    for (int i = 0; i < waveguide->num_nodes; i++) {
        destroy_node(&(waveguide->nodes[i]));
    }
    free(waveguide);
}

void run_waveguide(NN_Waveguide *waveguide) {
    // calculate next state
    for (int i = 0; i < waveguide->num_nodes; i++) {
        NN_Node *node = &(waveguide->nodes[i]);
        if (node->type != DRAIN) {
            for (int j = 0; j < node->num_links; j++) {
                move_energy(&(node->links[j]));
            }
        }
    }

    // flush changes
    for (int i = 0; i < waveguide->num_nodes; i++) {
        NN_Node *node = &(waveguide->nodes[i]);
        for (int j = 0; j < node->num_links; j++) {
            flush_link(&(node->links[j]));
        }
    }
}

NN_Node *spawn_node(NN_Waveguide *waveguide, NN_NodeType type) {
    NN_Node *node = create_node(type, 1);
    waveguide->nodes = append(waveguide->nodes, &waveguide->num_nodes, sizeof(NN_Node), node);
    return node;
}

NN_Link *link_nodes(NN_Node *a, NN_Node *b) {
    NN_Link *link_a = create_link(a, b);
    NN_Link *link_b = create_link(b, a);
    a->links = append(a->links, &a->num_links, sizeof(NN_Link), link_a);
    b->links = append(b->links, &b->num_links, sizeof(NN_Link), link_b);
    return link_a;
}

uint8_t *serialize(NN_Waveguide *waveguide) {
    // TODO
    return NULL;
}

NN_Waveguide *deserialize(uint8_t *data) {
    // TODO
    return NULL;
}
