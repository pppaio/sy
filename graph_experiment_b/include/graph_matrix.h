#ifndef GRAPH_MATRIX_H
#define GRAPH_MATRIX_H

#include "graph_interface.h"

typedef struct {
    GraphVertex vertexTable[MAX_VERTEX_COUNT];
    int weightMatrix[MAX_VERTEX_COUNT][MAX_VERTEX_COUNT];
    int vertexCount;
    int isDirected;
} MatrixGraphImpl;

#endif
