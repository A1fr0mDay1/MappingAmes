#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

#define INITIAL_CAPACITY 100

/**
 * Helper function to retrieve a node by its ID.
 */
node_t *findNode(graph_t *graph, int id) {
    if (graph == NULL) {
        return NULL;
    }
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i] != NULL && graph->nodes[i]->id == id) {
            return graph->nodes[i];
        }
    }
    return NULL;
}

/**
 * Creates a new graph and returns a pointer to it.
 */
graph_t *createGraph() {
    graph_t *graph = (graph_t *)malloc(sizeof(graph_t));
    if (graph == NULL) {
        return NULL;
    }

    graph->nodeCount = 0;
    graph->edgeCount = 0;
    graph->nodeSpace = INITIAL_CAPACITY;

    graph->nodes = (node_t **)malloc(sizeof(node_t *) * graph->nodeSpace);
    if (graph->nodes == NULL) {
        free(graph);
        return NULL;
    }

    return graph;
}

/**
 * Frees the memory used by the graph.
 */
void freeGraph(graph_t *graph) {
    if (graph == NULL) {
        return;
    }

    //Loop through every node
    for (int i = 0; i < graph->nodeCount; i++) {
        node_t *node = graph->nodes[i];
        if (node != NULL) {
            edge_t *edge = node->edges;
            while (edge != NULL) {
                edge_t *nextEdge = edge->next; 
                free(edge);
                edge = nextEdge;
            }

            free(node);
        }
    }

    free(graph->nodes);

    free(graph);
}

/**
 * Adds a new node to the graph.
 */
node_t *addNode(graph_t *graph, int id, void *data) {
    if (graph == NULL) {
        return NULL;
    }
    // Check if a node with the same ID already exists
    if (findNode(graph, id) != NULL) {
        return NULL;
    }


    if (graph->nodeCount == graph->nodeSpace) {
        int newSpace = graph->nodeSpace * 2;
        node_t **newNodes = (node_t **)realloc(graph->nodes, sizeof(node_t *) * newSpace);
        
        if (newNodes == NULL) {
            return NULL;
        }
        graph->nodes = newNodes;
        graph->nodeSpace = newSpace;
    }

    //Create the new node
    node_t *newNode = (node_t *)malloc(sizeof(node_t));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->id = id;
    newNode->data = data;
    newNode->edges = NULL;

    graph->nodes[graph->nodeCount] = newNode;
    graph->nodeCount++;

    return newNode;
}

/**
 * Retrieves a node from the graph by its ID.
 */
node_t *getNode(graph_t *graph, int id) {
    return findNode(graph, id);
}

/**
 * Retrieves an edge from the graph by its from and to node IDs.
 */
edge_t *getEdge(graph_t *graph, int fromId, int toId) {
    //Find the from node
    node_t *fromNode = findNode(graph, fromId);
    if (fromNode == NULL) {
        return NULL;
    }

    //Traverse the adjacency list to find the edge
    edge_t *edge = fromNode->edges;
    while (edge != NULL) {
        if (edge->toNode != NULL && edge->toNode->id == toId) {
            return edge;
        }
        edge = edge->next;
    }

    return NULL;
}


/**
 * Adds a new edge to the graph.
 */
edge_t *addEdge(graph_t *graph, int fromId, int toId, float weight, void *data) {
    if (graph == NULL) {
        return NULL;
    }

    //Find the from and to nodes
    node_t *fromNode = findNode(graph, fromId);
    node_t *toNode = findNode(graph, toId);

    if (fromNode == NULL || toNode == NULL || getEdge(graph, fromId, toId) != NULL) {
        return NULL;
    }

    //Create the new edge
    edge_t *newEdge = (edge_t *)malloc(sizeof(edge_t));
    if (newEdge == NULL) {
        return NULL;
    }

    newEdge->toNode = toNode;
    newEdge->weight = weight;
    newEdge->data = data;

    newEdge->next = fromNode->edges; 
    fromNode->edges = newEdge; 

    graph->edgeCount++;

    return newEdge;
}

/**
 * Removes a node from the graph.
 */
int removeNode(graph_t *graph, int id) {
    if (graph == NULL) {
        return 0;
    }

    //Find the node and its index
    int nodeIndex = -1;
    node_t *nodeToRemove = NULL;
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i]->id == id) {
            nodeIndex = i;
            nodeToRemove = graph->nodes[i];
            break;
        }
    }

    if (nodeToRemove == NULL) {
        return 0; 
    }

    //Remove all outgoing edges from this node
    edge_t *outEdge = nodeToRemove->edges;
    while (outEdge != NULL) {
        edge_t *next = outEdge->next;
        free(outEdge);
        graph->edgeCount--;
        outEdge = next;
    }
    nodeToRemove->edges = NULL;

    //Remove all incoming edges pointing to this node
    for (int i = 0; i < graph->nodeCount; i++) {
        if (i == nodeIndex) {
            continue;
        }
        
        node_t *otherNode = graph->nodes[i];
        edge_t *current = otherNode->edges;
        edge_t *prev = NULL;

        while (current != NULL) {
            if (current->toNode == nodeToRemove) {
                edge_t *next = current->next;
                if (prev == NULL) {
                    otherNode->edges = next;
                } 
                else {
                    prev->next = next;
                }
                free(current);
                graph->edgeCount--;
                current = next; 
            } 
            else {
                prev = current;
                current = current->next;
            }
        }
    }

    free(nodeToRemove);

    for (int i = nodeIndex; i < graph->nodeCount - 1; i++) {
        graph->nodes[i] = graph->nodes[i + 1];
    }
    graph->nodes[graph->nodeCount - 1] = NULL;
    graph->nodeCount--;

    return 1;
}

/**
 * Removes an edge from the graph.
 */
int removeEdge(graph_t *graph, int fromId, int toId) {
    if (graph == NULL) {
        return 0;
    }

    //Find the from node
    node_t *fromNode = findNode(graph, fromId);
    if (fromNode == NULL) {
        return 0;
    }

    edge_t *current = fromNode->edges;
    edge_t *prev = NULL;

    while (current != NULL) {
        if (current->toNode != NULL && current->toNode->id == toId) {
            if (prev == NULL) {
                fromNode->edges = current->next;
            } 
            else {
                prev->next = current->next;
            }
            
            free(current);
            graph->edgeCount--;
            return 1;
        }
        
        prev = current;
        current = current->next;
    }

    return 0;
}

/**
 * Prints the entire graph to the console.
 */
void printGraph(graph_t *graph) {
    if (graph == NULL) {
        return;
    }

    for (int i = 0; i < graph->nodeCount; i++) {
        node_t *node = graph->nodes[i];
        if (node == NULL) {
            continue;
        }

        // Print node header
        printf("Node %d: (data)\n", node->id);

        edge_t *edge = node->edges;
        if (edge == NULL) {
            printf("  (no outgoing edges)\n");
        } 
        else {

            while (edge != NULL) {
                if (edge->toNode != NULL) {
                    printf("  -> Node %d (weight: %.1f, data)\n", 
                           edge->toNode->id, 
                           edge->weight);
                }
                edge = edge->next;
            }
        }
    }
}