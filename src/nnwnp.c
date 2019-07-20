#include <nnwnp.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

double noise() {
    return rand() / (RAND_MAX / 2.0) - 1;
}

// destroy and remove reciprocal links
void destroy_reciprocal_links(NN_Link *link) {
    List *target_links = link->target->links;
    List *target_prev = NULL;
    while (target_links != NULL) {
        NN_Link *target_link = (NN_Link *)list_get(target_links, 0);
        List *next = target_links->next;
        if (target_link->target == link->source) {
            free(target_link);
            free(target_links);
            if (target_prev)
                target_prev->next = next;
            else
                link->target->links = next;
        }
        target_prev = target_links;
        target_links = next;
    }
}

NN_Node *create_node(NN_NodeType type, double area) {
    NN_Node *node = (NN_Node *)malloc(sizeof(NN_Node));
    node->type = type;
    node->area = area;
    node->links = NULL;
    return node;
}

void destroy_node(NN_Node *node) {
    while (node->links != NULL) {
        NN_Link *link = (NN_Link *)list_get(node->links, 0);
        destroy_reciprocal_links(link);
        free(link);
        list_remove(&(node->links), 0);
    }
    free(node);
}

double net_node_energy(NN_Node *node) {
    double energy = 0;
    List *links = node->links;
    while (links != NULL) {
        NN_Link *link = (NN_Link *)list_get(links, 0);
        energy += link->energy;
        links = links->next;
    }
    return energy;
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
    // destroy reciprocal links
    destroy_reciprocal_links(link);

    // remove from source list
    List *links = link->source->links;
    List *prev = NULL;
    while (links != NULL) {
        NN_Link *source_link = (NN_Link *)list_get(links, 0);
        List *next = links->next;
        if (source_link == link) {
            free(links);
            if (prev)
                prev->next = next;
            else
                link->source->links = next;
            break;
        }
        prev = links;
        links = next;
    }
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
void distribute_energy(NN_Waveguide *waveguide, NN_Node *node, double energy, NN_Node *source) {
    // calculate the net admittance away from the target junction
    double admittance = 0;

    // the link to store reflection energy
    NN_Link *reflection_link = NULL;

    List *links;

    links = node->links;
    while (links != NULL) {
        NN_Link *jlink = (NN_Link *)list_get(links, 0);
        NN_Node *jtarget = jlink->target;

        if (jtarget != source) {
            admittance += get_admittance(jtarget);
        } else {
            reflection_link = jlink;
        }

        links = links->next;
    }

    if (reflection_link) {
        // get the reflected energy
        double gamma = reflection(get_admittance(node), admittance);
        double reflection = gamma * energy;

        // get the turbulence
        double turbulence = fmax(0, reflection) * noise() * waveguide->turbulence;

        // send the reflected energy backward
        add_energy(reflection_link, (reflection + turbulence) * (1 - waveguide->damping));

        // subtract from remaining energy to be distributed
        energy -= reflection;
    }

    // distribute the remainder of the energy
    links = node->links;
    while (links != NULL) {
        NN_Link *jlink = (NN_Link *)list_get(links, 0);

        if (jlink != reflection_link) {
            // get the weight for this link

            double weight;
            if (admittance == INFINITY) {
                weight = 0;
            }
            else if (admittance == 0) {
                weight = 1;
            }
            else {
                weight = get_admittance(jlink->target) / admittance;
            }

            // move the weighted amonut of energy
            add_energy(jlink, weight * energy);
        }

        links = links->next;
    }
}

void move_energy(NN_Waveguide *waveguide, NN_Link *link) {
    distribute_energy(waveguide, link->target, link->energy, link->source);
}

void inject_energy(NN_Waveguide *waveguide, NN_Node *node, double energy) {
    distribute_energy(waveguide, node, energy, NULL);
}

double net_waveguide_energy(NN_Waveguide *waveguide) {
    double energy = 0;
    List *nodes = waveguide->nodes;
    while (nodes != NULL) {
        NN_Node *node = (NN_Node *)list_get(nodes, 0);
        energy += net_node_energy(node);
        nodes = nodes->next;
    }
    return energy;
}

NN_Waveguide *create_waveguide() {
    NN_Waveguide *waveguide = (NN_Waveguide *)malloc(sizeof(NN_Waveguide));
    waveguide->nodes = NULL;
    waveguide->damping = DAMPING;
    waveguide->turbulence = TURBULENCE;
    return waveguide;
}

void destroy_waveguide(NN_Waveguide *waveguide) {
    while (waveguide->nodes != NULL) {
        NN_Node *node = (NN_Node *)list_get(waveguide->nodes, 0);
        destroy_node(node);
        list_remove(&(waveguide->nodes), 0);
    }
    free(waveguide);
}

void run_waveguide(NN_Waveguide *waveguide) {
    List *nodes;

    // calculate next state
    nodes = waveguide->nodes;
    while (nodes != NULL) {
        NN_Node *node = (NN_Node *)list_get(nodes, 0);
        if (node->type != DRAIN) {
            List *links = node->links;
            while (links != NULL) {
                NN_Link *link = (NN_Link *)list_get(links, 0);
                move_energy(waveguide, link);
                links = links->next;
            }
        }
        nodes = nodes->next;
    }

    // flush changes
    nodes = waveguide->nodes;
    while (nodes != NULL) {
        NN_Node *node = (NN_Node *)list_get(nodes, 0);
        List *links = node->links;
        while (links != NULL) {
            NN_Link *link = (NN_Link *)list_get(links, 0);
            flush_link(link);
            links = links->next;
        }
        nodes = nodes->next;
    }
}

NN_Node *spawn_node(NN_Waveguide *waveguide, NN_NodeType type) {
    NN_Node *node = create_node(type, 1);
    list_append(&(waveguide->nodes), node);
    return node;
}

NN_Link *link_nodes(NN_Node *a, NN_Node *b) {
    NN_Link *link_a = create_link(a, b);
    NN_Link *link_b = create_link(b, a);
    list_append(&(a->links), link_a);
    list_append(&(b->links), link_b);
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
