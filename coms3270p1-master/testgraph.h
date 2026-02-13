#ifndef TESTGRAPH_H
#define TESTGRAPH_H

#include "graph.h"

/**
 * A struct to hold all the data for a Point of Interest (POI).
 * Create one of these for each POI and store a pointer to it
 * in the node_t's (void* data) field.
 */
typedef struct {
    char *name;
    double latitude;
    double longitude;
} poi_data_t;

#endif // TESTGRAPH_H