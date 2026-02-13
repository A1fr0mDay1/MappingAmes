#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>// For isnan()
#include "graph.h"
#include "testgraph.h"

#define MAX_LINE_LEN 1024

/**
 * Helper function to free all custom data from the graph
 * before the graph itself is freed.
 */
void freeCustomGraphData(graph_t *graph) {
    if (graph == NULL) return;

    // Loop through every node 
    for (int i = 0; i < graph->nodeCount; i++) {
        node_t *node = graph->nodes[i];
        if (node != NULL) {
            if (node->data != NULL) {
                poi_data_t *poi_data = (poi_data_t *)node->data;
                free(poi_data->name);
                free(poi_data);
            }

            // Loop through every edge from this node
            edge_t *edge = node->edges;
            while (edge != NULL) {
                if (edge->data != NULL) {
                    free(edge->data); 
                }
                edge = edge->next;
            }
        }
    }
}


int main() {
    graph_t *graph = createGraph();
    if (graph == NULL) {
        fprintf(stderr, "Failed to create graph\n");
        return 1;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    int numPoi = 0;
    int numRoads = 0;

    // --- 1. READ POI COUNT ---
    if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
        fprintf(stderr, "Error reading POI count\n");
        freeGraph(graph);
        return 1;
    }
    line_num = 1;
    sscanf(line, "%d", &numPoi);

    // --- 2. READ POI SECTION ---
    for (int i = 0; i < numPoi; i++) {
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
            fprintf(stderr, "Error: File ended early in POI section\n");
            freeGraph(graph);
            return 1;
        }
        line_num++;

        char poi_id_str[MAX_LINE_LEN];
        char poi_name[MAX_LINE_LEN];
        double lat;
        double lon;

        if (sscanf(line, "%[^\t]\t%[^\t]\t%lf\t%lf", poi_id_str, poi_name, &lat, &lon) != 4) {
            fprintf(stderr, "Error parsing POI on line %d\n", line_num);
            continue;
        }

        poi_data_t *poi_data = (poi_data_t *)malloc(sizeof(poi_data_t));

        if (poi_data == NULL) {
            fprintf(stderr, "Error: Malloc failed for poi_data\n");
            freeGraph(graph);
            return 1;
        }
        poi_data->name = strdup(poi_name);
        poi_data->latitude = lat;
        poi_data->longitude = lon;

        if (addNode(graph, atol(poi_id_str), (void *)poi_data) == NULL) {
            fprintf(stderr, "Error: Failed to add node %s\n", poi_id_str);
            free(poi_data->name);
            free(poi_data);
        }
    }

    // --- 3. READ ROAD COUNT ---
    if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
        fprintf(stderr, "Error reading road count\n");
        freeGraph(graph);
        return 1;
    }
    line_num++;
    sscanf(line, "%d", &numRoads);

    // --- 4. READ ROAD SECTION ---
    for (int i = 0; i < numRoads; i++) {
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
            fprintf(stderr, "Error: File ended early in road section\n");
            freeGraph(graph);
            return 1;
        }

        line_num++;

        char road_id_str[MAX_LINE_LEN];
        char poi_id_str[MAX_LINE_LEN];
        double distance;
        double lat;
        double lon;
        char road_name[MAX_LINE_LEN];

        if (sscanf(line, "%[^\t]\t%[^\t]\t%lf\t%lf\t%lf\t%[^\n]", road_id_str, poi_id_str, &distance, &lat, &lon, road_name) != 6) {
            fprintf(stderr, "Error parsing road on line %d\n", line_num);
            continue;
        }

        if (isnan(distance)) {
            distance = 0.0;
        }

        long from_id = atol(road_id_str);
        long to_id = atol(poi_id_str);
        
        if (getNode(graph, from_id) == NULL) {
            if (addNode(graph, from_id, NULL) == NULL) {
                fprintf(stderr, "Fatal: Failed to add generic node %ld (from)\n", from_id);
                continue;
            }
        }

        if (getNode(graph, to_id) == NULL) {
            if (addNode(graph, to_id, NULL) == NULL) {
                fprintf(stderr, "Fatal: Failed to add generic node %ld (to)\n", to_id);
                continue;
            }
        }

        char *road_name_copy = strdup(road_name);
        if (road_name_copy == NULL) {
             fprintf(stderr, "Error: Malloc failed for road name\n");
             continue;
        }

        if (addEdge(graph, from_id, to_id, (float)distance, (void *)road_name_copy) == NULL) {
            free(road_name_copy);
        }
    }

    // --- 5. PRINT AND CLEAN UP ---
    printGraph(graph);

    // Free all custom data because freeGraph only frees graph structure
    freeCustomGraphData(graph);
    
    freeGraph(graph);

    return 0;
}