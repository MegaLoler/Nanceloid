#include <waveguide.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// produce a normalized random value between -1 and 1
double noise () {
    return rand () / (RAND_MAX / 2.0) - 1;
}

// destroy and remove reciprocal links
void destroy_reciprocal_links (NN_Link *link) {
    List *target_links = link->target->links;
    List *target_prev = NULL;
    while (target_links != NULL) {
        // TODO: optimize getting list node contents
        NN_Link *target_link = (NN_Link *) list_get (target_links, 0);
        List *next = target_links->next;
        if (target_link->target == link->source) {
            free (target_link);
            free (target_links);
            if (target_prev)
                target_prev->next = next;
            else
                link->target->links = next;
        }
        target_prev = target_links;
        target_links = next;
    }
}

NN_Node *create_node (double area) {
    NN_Node *node = (NN_Node *) malloc(sizeof(NN_Node));
    node->area = area;
    node->links = NULL;
    return node;
}

void destroy_node (NN_Node *node) {
    if (node == NULL)
        return;
    while (node->links != NULL) {
        // TODO: optimize getting list node contents
        NN_Link *link = (NN_Link *) list_get(node->links, 0);
        destroy_reciprocal_links (link);
        free (link);
        list_remove (&(node->links), 0);
    }
    free (node);
}

double net_node_energy (NN_Node *node) {
    double energy = 0;
    List *links = node->links;
    while (links != NULL) {
        // TODO: optimize getting list node contents
        NN_Link *link = (NN_Link *) list_get(links, 0);
        energy += link->energy;
        links = links->next;
    }
    return energy;
}

double area_to_admittance (double area) {
    return area;
}

double admittance_to_area (double Y) {
    return Y;
}

double get_impedance (NN_Node *node) {
    return 1 / get_admittance (node);
}

double get_admittance (NN_Node *node) {
    return area_to_admittance (node->area);
}

void set_impedance (NN_Node *node, double Z) {
    set_admittance (node, 1 / Z);
}

void set_admittance (NN_Node *node, double Y) {
    node->area = admittance_to_area(Y);
}

NN_Link *create_link (NN_Node *source, NN_Node *target) {
    NN_Link *link = (NN_Link *) malloc (sizeof (NN_Link));
    link->source = source;
    link->target = target;
    link->energy = 0;
    link->queued = 0;
    return link;
}

void destroy_link (NN_Link *link) {
    // destroy reciprocal links
    destroy_reciprocal_links (link);

    // remove from source list
    List *links = link->source->links;
    List *prev = NULL;
    while (links != NULL) {
        // TODO: optimize getting list node contents
        NN_Link *source_link = (NN_Link *) list_get(links, 0);
        List *next = links->next;
        if (source_link == link) {
            free (links);
            if (prev)
                prev->next = next;
            else
                link->source->links = next;
            break;
        }
        prev = links;
        links = next;
    }
    free (link);
}

void add_energy (NN_Link *link, double energy) {
    link->queued += energy;
}

void flush_link (NN_Link *link) {
    link->energy = link->queued;
    link->queued = 0;
}

double reflection (double source_Y, double target_Y) {
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

// weighted distribution of energy into target node's links
// but dont distribute back to source
// return any energy that was not distributed
// (such as if there is no where for it to go)
double distribute_energy (NN_Node *target, NN_Node *source, double energy) {

    List *links;

    // calculate the net admittance away from the target junction
    double admittance = 0;
    links = target->links;
    while (links != NULL) {
        NN_Link *link = (NN_Link *) list_get (links, 0);

        // don't distribute back to source
        //if (link->target != source)
            admittance += get_admittance (link->target);

        links = links->next;
    }

    // distribute with weights
    double total = 0; // total distributed energy
    links = target->links;
    while (links != NULL) {
        NN_Link *link = (NN_Link *) list_get (links, 0);

        // don't distribute back to source
        //if (link->target != source) {
            double link_admittance = get_admittance (link->target);

            double weight;
            if (admittance == INFINITY)
                weight = link_admittance == INFINITY ? 1 : 0;
            else if (admittance == 0)
                weight = 0;
            else
                weight = link_admittance / admittance;

            double distribution = weight * energy;
            total += distribution;
            add_energy (link, distribution);
        //}

        links = links->next;
    }

    // return drain
    return energy - total;
}

void move_energy (NN_Waveguide *waveguide, NN_Link *link) {
    // calculate reflected energy
    double gamma = reflection (get_admittance (link->source), get_admittance (link->target));
    double reflection = gamma * link->energy;

    // generate some turbulence
    double turbulence = fmax (0, reflection) * noise () * waveguide->turbulence;

    // send the reflected energy backward
    double total_reflection = (reflection + turbulence) * (1 - waveguide->damping);
    distribute_energy (link->source, link->target, total_reflection);

    // distribute remaining energy to target and collect drain
    double remainder = link->energy - reflection;
    waveguide->drain += distribute_energy (link->target, link->source, remainder);
}

void inject_energy (NN_Waveguide *waveguide, NN_Node *node, double energy) {
    distribute_energy (node, NULL, energy);
}

double net_waveguide_energy (NN_Waveguide *waveguide) {
    double energy = 0;
    List *nodes = waveguide->nodes;
    while (nodes != NULL) {
        // TODO: optimize getting list node contents
        NN_Node *node = (NN_Node *) list_get(nodes, 0);
        energy += net_node_energy (node);
        nodes = nodes->next;
    }
    return energy;
}

double collect_drain (NN_Waveguide *waveguide) {
    double drain = waveguide->drain;
    waveguide->drain = 0;
    return drain;
}

NN_Waveguide *create_waveguide () {
    NN_Waveguide *waveguide = (NN_Waveguide *) malloc (sizeof (NN_Waveguide));
    waveguide->nodes = NULL;
    waveguide->damping = DAMPING;
    waveguide->turbulence = TURBULENCE;
    waveguide->drain = 0;
    return waveguide;
}

void destroy_waveguide (NN_Waveguide *waveguide) {
    if (waveguide == NULL)
        return;
    while (waveguide->nodes != NULL) {
        // TODO: optimize getting list node contents
        NN_Node *node = (NN_Node *) list_get (waveguide->nodes, 0);
        destroy_node (node);
        list_remove (&(waveguide->nodes), 0);
    }
    free (waveguide);
}

void run_waveguide (NN_Waveguide *waveguide) {
    List *nodes;

    // calculate next state
    nodes = waveguide->nodes;
    while (nodes != NULL) {
        // TODO: optimize getting list node contents
        NN_Node *node = (NN_Node *) list_get (nodes, 0);
        List *links = node->links;
        while (links != NULL) {
            // TODO: optimize getting list node contents
            NN_Link *link = (NN_Link *) list_get (links, 0);
            move_energy (waveguide, link);
            links = links->next;
        }
        nodes = nodes->next;
    }

    // flush changes
    nodes = waveguide->nodes;
    while (nodes != NULL) {
        // TODO: optimize getting list node contents
        NN_Node *node = (NN_Node *) list_get (nodes, 0);
        List *links = node->links;
        while (links != NULL) {
            // TODO: optimize getting list node contents
            NN_Link *link = (NN_Link *) list_get (links, 0);
            flush_link (link);
            links = links->next;
        }
        nodes = nodes->next;
    }
}

NN_Node *spawn_node (NN_Waveguide *waveguide) {
    NN_Node *node = create_node (1);
    list_append (&(waveguide->nodes), node);
    return node;
}

NN_Link *single_link_nodes (NN_Node *source, NN_Node *target) {
    NN_Link *link = create_link (source, target);
    list_append (&(source->links), link);
    return link;
}

NN_Link *double_link_nodes (NN_Node *source, NN_Node *target) {
    single_link_nodes (target, source);
    return single_link_nodes (source, target);
}

NN_Link *terminate_node (NN_Node *node) {
    return single_link_nodes (node, node);
}

uint8_t *serialize (NN_Waveguide *waveguide) {
    // TODO
    return NULL;
}

NN_Waveguide *deserialize (uint8_t *data) {
    // TODO
    return NULL;
}
