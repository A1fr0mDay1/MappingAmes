#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "graph.h"
#include "data.h"

// Earth radius in meters for Haversine formula
#define EARTH_RADIUS 6371000.0
#define MAX_LINE_LEN 1024

// POI data structure
typedef struct {
    char *name;
    double latitude;
    double longitude;
} poi_data_t;

// Function prototypes
void printUsage(char *programName);
graph_t* loadFileGraph(char *filename);
double calculateDistance(double lat1, double lon1, double lat2, double lon2);
void findLocation(graph_t *graph, char *locationName);
void findDiameter(graph_t *graph);
void distanceBetween(graph_t *graph, char *name1, char *name2);
void roadDistance(graph_t *graph, char *name1, char *name2);
double dijkstra(graph_t *graph, int startId, int endId);
void freeGraphWithData(graph_t *graph);
node_t* findNodeByName(graph_t *graph, char *name);

/**
 * Print usage statement
 */
void printUsage(char *programName) {
    printf("Usage: %s -f <filename> [options]\n", programName);
    printf("Options:\n");
    printf("  -f <filename>              Load city data from file (required)\n");
    printf("  -location <name>           Find location and print lat/long\n");
    printf("  -diameter                  Find max distance between any two nodes\n");
    printf("  -distance <name1> <name2>  Calculate straight-line distance\n");
    printf("  -roaddist <name1> <name2>  Calculate shortest road distance\n");
}

/**
 * Calculate distance between two lat/lon points using Haversine formula
 */
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    double lat1_rad;
    double lat2_rad;
    double dlat;
    double dlon;
    double a;
    double c;
    
    // Convert to radians
    lat1_rad = lat1 * M_PI / 180.0;
    lat2_rad = lat2 * M_PI / 180.0;
    dlat = (lat2 - lat1) * M_PI / 180.0;
    dlon = (lon2 - lon1) * M_PI / 180.0;
    
    // Haversine formula
    a = sin(dlat/2) * sin(dlat/2) + cos(lat1_rad) * cos(lat2_rad) * sin(dlon/2) * sin(dlon/2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    return EARTH_RADIUS * c;
}

/**
 * Load graph from file
 */
graph_t* loadFileGraph(char *filename) {
    FILE *file;
    FILE *oldStdin;

    graph_t *graph;

    char line[MAX_LINE_LEN];
    char id_str[MAX_LINE_LEN];
    char name[MAX_LINE_LEN];
    char from_id[MAX_LINE_LEN];
    char to_id[MAX_LINE_LEN];
    char road_name[MAX_LINE_LEN];
    char *road_name_copy;

    int validationResult;
    int numPoi;
    int numRoads;
    int i;
    int id;
    int from;
    int to;

    poi_data_t *poi_data;
    poi_data_t *intersection;
    
    double lat;
    double lon;
    double distance;
    
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    oldStdin = stdin;
    stdin = file;
    
    validationResult = validate();
    if (validationResult != 0) {
        fprintf(stderr, "Error: Invalid file format at line %d\n", validationResult);
        stdin = oldStdin;
        fclose(file);
        return NULL;
    }
    
    fseek(file, 0, SEEK_SET);
    
    graph = createGraph();
    if (!graph) {
        stdin = oldStdin;
        fclose(file);
        return NULL;
    }
    
    if (fgets(line, MAX_LINE_LEN, file) == NULL) {
        freeGraph(graph);
        stdin = oldStdin;
        fclose(file);
        return NULL;
    }
    
    sscanf(line, "%d", &numPoi);
    
    // Read POIs
    for (i = 0; i < numPoi; i++) {
        if (fgets(line, MAX_LINE_LEN, file) == NULL) {
            freeGraphWithData(graph);
            stdin = oldStdin;
            fclose(file);
            return NULL;
        }
        
        if (sscanf(line, "%[^\t]\t%[^\t]\t%lf\t%lf", id_str, name, &lat, &lon) != 4) {
            continue;
        }
        
        poi_data = (poi_data_t*)malloc(sizeof(poi_data_t));
        if (!poi_data) {
            freeGraphWithData(graph);
            stdin = oldStdin;
            fclose(file);
            return NULL;
        }
        
        poi_data->name = strdup(name);
        poi_data->latitude = lat;
        poi_data->longitude = lon;
        
        id = atoi(id_str);
        if (addNode(graph, id, poi_data) == NULL) {
            free(poi_data->name);
            free(poi_data);
        }
    }
    
    // Read road count
    if (fgets(line, MAX_LINE_LEN, file) == NULL) {
        freeGraphWithData(graph);
        stdin = oldStdin;
        fclose(file);
        return NULL;
    }
    
    sscanf(line, "%d", &numRoads);
    
    // Read roads
    for (i = 0; i < numRoads; i++) {
        if (fgets(line, MAX_LINE_LEN, file) == NULL) {
            freeGraphWithData(graph);
            stdin = oldStdin;
            fclose(file);
            return NULL;
        }
        
        if (sscanf(line, "%[^\t]\t%[^\t]\t%lf\t%lf\t%lf\t%[^\n]", from_id, to_id, &distance, &lat, &lon, road_name) != 6) {
            continue;
        }
        
        if (isnan(distance)) {
            distance = 0.0;
        }
        
        from = atoi(from_id);
        to = atoi(to_id);
        
        // Add nodes that do not exist yet
        if (getNode(graph, from) == NULL) {
            intersection = (poi_data_t*)malloc(sizeof(poi_data_t));
            if (intersection) {
                intersection->name = NULL;
                intersection->latitude = lat;
                intersection->longitude = lon;
                addNode(graph, from, intersection);
            }
        }
        
        if (getNode(graph, to) == NULL) {
            addNode(graph, to, NULL);
        }
        
        road_name_copy = strdup(road_name);
        if (addEdge(graph, from, to, (float)distance, road_name_copy) == NULL) {
            free(road_name_copy);
        }
    }
    
    stdin = oldStdin;
    fclose(file);
    return graph;
}

/**
 * Free graph including all custom data
 */
void freeGraphWithData(graph_t *graph) {
    int i;
    node_t *node;
    poi_data_t *poi;
    edge_t *edge;
    
    if (graph == NULL) return;
    
    // Free all custom data first
    for (i = 0; i < graph->nodeCount; i++) {
        node = graph->nodes[i];
        if (node != NULL) {
            if (node->data != NULL) {
                poi = (poi_data_t*)node->data;
                if (poi->name != NULL) {
                    free(poi->name);
                }
                free(poi);
            }
            
            edge = node->edges;
            while (edge != NULL) {
                if (edge->data != NULL) {
                    free(edge->data);
                }
                edge = edge->next;
            }
        }
    }
    
    freeGraph(graph);
}

/**
 * Find a node by POI name
 */
node_t* findNodeByName(graph_t *graph, char *name) {
    int i;
    node_t *node;
    poi_data_t *poi;
    
    for (i = 0; i < graph->nodeCount; i++) {
        node = graph->nodes[i];
        if (node != NULL && node->data != NULL) {
            poi = (poi_data_t*)node->data;
            if (poi->name != NULL && strcmp(poi->name, name) == 0) {
                return node;
            }
        }
    }
    return NULL;
}

/**
 * Find and print location coordinates
 */
void findLocation(graph_t *graph, char *locationName) {
    node_t *node;
    poi_data_t *poi;
    
    node = findNodeByName(graph, locationName);
    if (node != NULL) {
        poi = (poi_data_t*)node->data;
        printf("%.4f %.4f\n", poi->latitude, poi->longitude);
    } 
    else {
        fprintf(stderr, "Error: Location '%s' not found\n", locationName);
    }
}

/**
 * Find the diameter (maximum distance between any two POIs)
 */
void findDiameter(graph_t *graph) {
    double maxDistance;
    int node1_idx;
    int node2_idx;
    int i;
    int j;

    node_t *node1;
    node_t *node2;

    poi_data_t *poi1;
    poi_data_t *poi2;

    double dist;
    
    maxDistance = 0;
    node1_idx = -1;
    node2_idx = -1;
    
    // Only consider POI nodes
    for (i = 0; i < graph->nodeCount; i++) {
        node1 = graph->nodes[i];
        if (node1 == NULL || node1->data == NULL) continue;
        poi1 = (poi_data_t*)node1->data;
        if (poi1->name == NULL) continue;
        
        for (j = i + 1; j < graph->nodeCount; j++) {
            node2 = graph->nodes[j];
            if (node2 == NULL || node2->data == NULL) continue;
            poi2 = (poi_data_t*)node2->data;
            if (poi2->name == NULL) continue;
            
            dist = calculateDistance(poi1->latitude, poi1->longitude, poi2->latitude, poi2->longitude);
            if (dist > maxDistance) {
                maxDistance = dist;
                node1_idx = i;
                node2_idx = j;
            }
        }
    }
    
    if (node1_idx != -1 && node2_idx != -1) {
        poi1 = (poi_data_t*)graph->nodes[node1_idx]->data;
        poi2 = (poi_data_t*)graph->nodes[node2_idx]->data;
        printf("%.4f %.4f %.4f %.4f %.2f\n", poi1->latitude, poi1->longitude, poi2->latitude, poi2->longitude, maxDistance);
    }
}

/**
 * Calculate distance between two named locations
 */
void distanceBetween(graph_t *graph, char *name1, char *name2) {
    node_t *node1;
    node_t *node2;
    poi_data_t *poi1; 
    poi_data_t *poi2;
    double dist;
    
    node1 = findNodeByName(graph, name1);
    node2 = findNodeByName(graph, name2);
    
    if (node1 == NULL || node2 == NULL) {
        fprintf(stderr, "Error: One or both locations not found\n");
        return;
    }
    
    poi1 = (poi_data_t*)node1->data;
    poi2 = (poi_data_t*)node2->data;
    
    dist = calculateDistance(poi1->latitude, poi1->longitude, poi2->latitude, poi2->longitude);
    printf("%.3f\n", dist);
}

/**
 * Dijkstra's algorithm implementation
 */
double dijkstra(graph_t *graph, int startId, int endId) {
    double *distances;
    double minDist;
    double alt;
    double result;

    int *visited;
    int startIdx;
    int endIdx;
    int i;
    int count;
    int minIdx;
    int neighborIdx;

    node_t *currentNode;
    edge_t *edge;  
    
    if (graph == NULL || graph->nodeCount == 0) {
        return -1;
    }
    
    // Allocate arrays
    distances = (double*)malloc(sizeof(double) * graph->nodeCount);
    visited = (int*)calloc(graph->nodeCount, sizeof(int));
    
    if (distances == NULL || visited == NULL) {
        free(distances);
        free(visited);
        return -1;
    }
    
    for (i = 0; i < graph->nodeCount; i++) {
        distances[i] = DBL_MAX;
    }
    
    startIdx = -1;
    for (i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i]->id == startId) {
            startIdx = i;
            distances[i] = 0;
            break;
        }
    }
    
    if (startIdx == -1) {
        free(distances);
        free(visited);
        return -1;
    }
    
    // Main Dijkstra loop
    for (count = 0; count < graph->nodeCount; count++) {
        minDist = DBL_MAX;
        minIdx = -1;
        
        for (i = 0; i < graph->nodeCount; i++) {
            if (!visited[i] && distances[i] < minDist) {
                minDist = distances[i];
                minIdx = i;
            }
        }
        
        if (minIdx == -1) break;
        
        visited[minIdx] = 1;
        
        if (graph->nodes[minIdx]->id == endId) {
            break;
        }
        
        currentNode = graph->nodes[minIdx];
        
        edge = currentNode->edges;
        while (edge != NULL) {
            neighborIdx = -1;
            for (i = 0; i < graph->nodeCount; i++) {
                if (graph->nodes[i] == edge->toNode) {
                    neighborIdx = i;
                    break;
                }
            }
            
            if (neighborIdx != -1 && !visited[neighborIdx]) {
                alt = distances[minIdx] + edge->weight;
                if (alt < distances[neighborIdx]) {
                    distances[neighborIdx] = alt;
                }
            }
            
            edge = edge->next;
        }
    }
    
    result = DBL_MAX;
    for (i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i]->id == endId) {
            result = distances[i];
            break;
        }
    }
    
    free(distances);
    free(visited);
    
    return (result == DBL_MAX) ? -1 : result;
}

/**
 * Calculate shortest road distance between two named locations
 */
void roadDistance(graph_t *graph, char *name1, char *name2) {
    node_t *node1;
    node_t *node2;

    double distance;
    
    node1 = findNodeByName(graph, name1);
    node2 = findNodeByName(graph, name2);
    
    if (node1 == NULL || node2 == NULL) {
        fprintf(stderr, "Error: One or both locations not found\n");
        return;
    }
    
    distance = dijkstra(graph, node1->id, node2->id);
    
    if (distance < 0) {
        fprintf(stderr, "Error: No path found between locations\n");
    } 
    else {
        printf("%.3f\n", distance);
    }
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
    char *filename;
    graph_t *graph;
    int i;
    
    // Check for no arguments
    if (argc == 1) {
        printUsage(argv[0]);
        return 0;
    }
    
    filename = NULL;
    for (i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            filename = argv[i + 1];
            break;
        }
    }
    
    if (filename == NULL) {
        fprintf(stderr, "Error: -f <filename> is required\n");
        printUsage(argv[0]);
        return 1;
    }
    
    graph = loadFileGraph(filename);
    if (graph == NULL) {
        return 1;
    }
    
    // Process other parameters IN ORDER THEY APPEAR
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            i++;
        } 
        else if (strcmp(argv[i], "-location") == 0) {
            if (i + 1 < argc) {
                findLocation(graph, argv[i + 1]);
                i++;
            } 
            else {
                fprintf(stderr, "Error: -location requires a location name\n");
            }
        } 
        else if (strcmp(argv[i], "-diameter") == 0) {
            findDiameter(graph);
        } 
        else if (strcmp(argv[i], "-distance") == 0) {
            if (i + 2 < argc) {
                distanceBetween(graph, argv[i + 1], argv[i + 2]);
                i += 2;
            } 
            else {
                fprintf(stderr, "Error: -distance requires two location names\n");
            }
        } 
        else if (strcmp(argv[i], "-roaddist") == 0) {
            if (i + 2 < argc) {
                roadDistance(graph, argv[i + 1], argv[i + 2]);
                i += 2;
            } 
            else {
                fprintf(stderr, "Error: -roaddist requires two location names\n");
            }
        }
    }
    
    freeGraphWithData(graph);
    
    return 0;
}