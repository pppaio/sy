#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/graph_matrix.h"

static MatrixGraphImpl *getImpl(IGraph *self)
{
    return (MatrixGraphImpl *)self->implData;
}

static const MatrixGraphImpl *getConstImpl(const IGraph *self)
{
    return (const MatrixGraphImpl *)self->implData;
}

static void initWeightMatrix(int weightMatrix[MAX_VERTEX_COUNT][MAX_VERTEX_COUNT])
{
    int rowIndex;
    int colIndex;
    for (rowIndex = 0; rowIndex < MAX_VERTEX_COUNT; rowIndex++) {
        for (colIndex = 0; colIndex < MAX_VERTEX_COUNT; colIndex++) {
            weightMatrix[rowIndex][colIndex] = (rowIndex == colIndex) ? 0 : INFINITE_WEIGHT;
        }
    }
}

static void safeStringCopy(char *dest, const char *src, size_t destSize)
{
    if (dest == NULL || destSize == 0) {
        return;
    }
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

static void matrixCreateGraph(IGraph *self, int isDirected)
{
    MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL) {
        return;
    }
    impl = getImpl(self);
    impl->vertexCount = 0;
    impl->isDirected = isDirected;
    initWeightMatrix(impl->weightMatrix);
    memset(impl->vertexTable, 0, sizeof(impl->vertexTable));
}

static void matrixDestroyGraph(IGraph *self)
{
    MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL) {
        return;
    }
    impl = getImpl(self);
    impl->vertexCount = 0;
    impl->isDirected = 0;
    initWeightMatrix(impl->weightMatrix);
    memset(impl->vertexTable, 0, sizeof(impl->vertexTable));
}

static int matrixAddVertex(IGraph *self, const char *vertexName, const char *vertexDesc)
{
    MatrixGraphImpl *impl;
    int newVertexId;
    if (self == NULL || self->implData == NULL || vertexName == NULL) {
        return -1;
    }
    impl = getImpl(self);
    if (impl->vertexCount >= MAX_VERTEX_COUNT) {
        printf("[addVertex] 顶点数量已达上限 %d，无法继续添加\n", MAX_VERTEX_COUNT);
        return -1;
    }
    newVertexId = impl->vertexCount;
    impl->vertexTable[newVertexId].vertexId = newVertexId;
    safeStringCopy(impl->vertexTable[newVertexId].vertexName, vertexName, MAX_VERTEX_NAME_LENGTH);
    safeStringCopy(impl->vertexTable[newVertexId].vertexDesc, vertexDesc, MAX_VERTEX_DESC_LENGTH);
    impl->vertexCount++;
    return newVertexId;
}

static int isValidVertexId(const MatrixGraphImpl *impl, int vertexId)
{
    return (impl != NULL && vertexId >= 0 && vertexId < impl->vertexCount);
}

static int matrixAddEdge(IGraph *self, int fromVertexId, int toVertexId, int edgeWeight)
{
    MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL) {
        return -1;
    }
    impl = getImpl(self);
    if (!isValidVertexId(impl, fromVertexId) || !isValidVertexId(impl, toVertexId)) {
        printf("[addEdge] 顶点ID超出范围: from=%d, to=%d, 当前顶点数=%d\n",
               fromVertexId, toVertexId, impl->vertexCount);
        return -1;
    }
    if (edgeWeight < 0) {
        printf("[addEdge] 权值不能为负: %d\n", edgeWeight);
        return -1;
    }
    impl->weightMatrix[fromVertexId][toVertexId] = edgeWeight;
    if (!impl->isDirected) {
        impl->weightMatrix[toVertexId][fromVertexId] = edgeWeight;
    }
    return 0;
}

static int matrixGetVertexCount(const IGraph *self)
{
    const MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL) {
        return 0;
    }
    impl = getConstImpl(self);
    return impl->vertexCount;
}

static int matrixGetVertexById(const IGraph *self, int vertexId, GraphVertex *outVertex)
{
    const MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL || outVertex == NULL) {
        return -1;
    }
    impl = getConstImpl(self);
    if (!isValidVertexId(impl, vertexId)) {
        return -1;
    }
    *outVertex = impl->vertexTable[vertexId];
    return 0;
}

static int matrixHasEdge(const IGraph *self, int fromVertexId, int toVertexId)
{
    const MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL) {
        return 0;
    }
    impl = getConstImpl(self);
    if (!isValidVertexId(impl, fromVertexId) || !isValidVertexId(impl, toVertexId)) {
        return 0;
    }
    return (impl->weightMatrix[fromVertexId][toVertexId] != INFINITE_WEIGHT &&
            fromVertexId != toVertexId);
}

static int matrixGetEdgeWeight(const IGraph *self, int fromVertexId, int toVertexId)
{
    const MatrixGraphImpl *impl;
    if (self == NULL || self->implData == NULL) {
        return INFINITE_WEIGHT;
    }
    impl = getConstImpl(self);
    if (!isValidVertexId(impl, fromVertexId) || !isValidVertexId(impl, toVertexId)) {
        return INFINITE_WEIGHT;
    }
    return impl->weightMatrix[fromVertexId][toVertexId];
}

static void printWeightCell(int weight, int cellWidth)
{
    if (weight == INFINITE_WEIGHT) {
        printf("%*s", cellWidth, "∞");
    } else {
        printf("%*d", cellWidth, weight);
    }
}

static void matrixDisplayGraph(const IGraph *self)
{
    const MatrixGraphImpl *impl;
    int rowIndex;
    int colIndex;
    int count;
    if (self == NULL || self->implData == NULL) {
        printf("[displayGraph] 空图指针\n");
        return;
    }
    impl = getConstImpl(self);
    count = impl->vertexCount;

    printf("\n==================== 景区路径图（邻接矩阵） ====================\n");
    printf("图类型: %s图 | 顶点数: %d\n", impl->isDirected ? "有向" : "无向", count);
    printf("∞ = 两景点间无直接路径\n");
    printf("----------------------------------------------------------------\n");

    printf("%10s", "");
    for (colIndex = 0; colIndex < count; colIndex++) {
        printf(" V%d:%-5s", colIndex, impl->vertexTable[colIndex].vertexName);
    }
    printf("\n");

    for (rowIndex = 0; rowIndex < count; rowIndex++) {
        printf("V%d:%-6s", rowIndex, impl->vertexTable[rowIndex].vertexName);
        for (colIndex = 0; colIndex < count; colIndex++) {
            printWeightCell(impl->weightMatrix[rowIndex][colIndex], 10);
        }
        printf("\n");
    }
    printf("===============================================================\n\n");
}

static void matrixDisplayVertexList(const IGraph *self)
{
    const MatrixGraphImpl *impl;
    int count;
    int vertexIndex;
    int neighborIndex;
    int hasNeighbor;
    if (self == NULL || self->implData == NULL) {
        printf("[displayVertexList] 空图指针\n");
        return;
    }
    impl = getConstImpl(self);
    count = impl->vertexCount;

    printf("\n==================== 景点链表（邻接表视图） ====================\n");
    printf("[HEAD]");
    for (vertexIndex = 0; vertexIndex < count; vertexIndex++) {
        printf(" ---> [V%d:%s]", vertexIndex, impl->vertexTable[vertexIndex].vertexName);
    }
    printf(" ---> [NULL]\n");
    printf("----------------------------------------------------------------\n");

    for (vertexIndex = 0; vertexIndex < count; vertexIndex++) {
        printf("  顶点链表#%d | V%d:%s",
               vertexIndex,
               vertexIndex,
               impl->vertexTable[vertexIndex].vertexName);
        if (strlen(impl->vertexTable[vertexIndex].vertexDesc) > 0) {
            printf(" (%s)", impl->vertexTable[vertexIndex].vertexDesc);
        }
        printf("\n    相邻: ");
        hasNeighbor = 0;
        for (neighborIndex = 0; neighborIndex < count; neighborIndex++) {
            if (vertexIndex != neighborIndex &&
                impl->weightMatrix[vertexIndex][neighborIndex] != INFINITE_WEIGHT) {
                if (hasNeighbor) {
                    printf(" -> ");
                }
                printf("[V%d:%s|%d]",
                       neighborIndex,
                       impl->vertexTable[neighborIndex].vertexName,
                       impl->weightMatrix[vertexIndex][neighborIndex]);
                hasNeighbor = 1;
            }
        }
        if (!hasNeighbor) {
            printf("(无相邻景点)");
        }
        printf("\n");
    }
    printf("===============================================================\n\n");
}

static void dfsRecursive(const MatrixGraphImpl *impl, int currentVertexId, int visited[], int path[], int edgeWeights[], int *pathIndex, int *totalDistance)
{
    int neighborIndex;
    visited[currentVertexId] = 1;
    path[*pathIndex] = currentVertexId;
    (*pathIndex)++;

    for (neighborIndex = 0; neighborIndex < impl->vertexCount; neighborIndex++) {
        if (currentVertexId != neighborIndex &&
            impl->weightMatrix[currentVertexId][neighborIndex] != INFINITE_WEIGHT &&
            !visited[neighborIndex]) {
            edgeWeights[*pathIndex - 1] = impl->weightMatrix[currentVertexId][neighborIndex];
            *totalDistance += impl->weightMatrix[currentVertexId][neighborIndex];
            dfsRecursive(impl, neighborIndex, visited, path, edgeWeights, pathIndex, totalDistance);
        }
    }
}

static void matrixDfsTraverse(const IGraph *self, int startVertexId)
{
    const MatrixGraphImpl *impl;
    int visited[MAX_VERTEX_COUNT];
    int path[MAX_VERTEX_COUNT];
    int edgeWeights[MAX_VERTEX_COUNT];
    int pathIndex = 0;
    int totalDistance = 0;
    int i;

    if (self == NULL || self->implData == NULL) {
        printf("[dfsTraverse] 空图指针\n");
        return;
    }
    impl = getConstImpl(self);

    if (!isValidVertexId(impl, startVertexId)) {
        printf("[dfsTraverse] 起始顶点ID %d 无效\n", startVertexId);
        return;
    }

    memset(visited, 0, sizeof(visited));
    memset(path, -1, sizeof(path));
    memset(edgeWeights, -1, sizeof(edgeWeights));

    printf("\n==================== DFS导游线路图 ====================\n");
    printf("起始景点: V%d(%s)\n", startVertexId, impl->vertexTable[startVertexId].vertexName);
    printf("------------------------------------------------------\n");

    dfsRecursive(impl, startVertexId, visited, path, edgeWeights, &pathIndex, &totalDistance);

    if (pathIndex > 0) {
        printf("导游线路: ");
        for (i = 0; i < pathIndex; i++) {
            printf("V%d(%s)", path[i], impl->vertexTable[path[i]].vertexName);
            if (i < pathIndex - 1) {
                if (edgeWeights[i] >= 0) {
                    printf(" --(%d)--> ", edgeWeights[i]);
                } else {
                    printf(" --(∞)--> ");
                }
            }
        }
        printf("\n覆盖景点数: %d\n", pathIndex);
        printf("总游览距离: %d 米\n", totalDistance);
    } else {
        printf("未找到可达景点\n");
    }
    printf("======================================================\n\n");
}

static int cycleRecursive(const MatrixGraphImpl *impl, int currentVertexId, int visited[], int inStack[], int path[], int *pathIndex, int *hasCycleFlag)
{
    int neighborIndex;
    visited[currentVertexId] = 1;
    inStack[currentVertexId] = 1;
    path[*pathIndex] = currentVertexId;
    (*pathIndex)++;

    for (neighborIndex = 0; neighborIndex < impl->vertexCount; neighborIndex++) {
        if (currentVertexId != neighborIndex &&
            impl->weightMatrix[currentVertexId][neighborIndex] != INFINITE_WEIGHT) {
            if (!visited[neighborIndex]) {
                if (cycleRecursive(impl, neighborIndex, visited, inStack, path, pathIndex, hasCycleFlag)) {
                    return 1;
                }
            } else if (inStack[neighborIndex]) {
                *hasCycleFlag = 1;
                return 1;
            }
        }
    }

    inStack[currentVertexId] = 0;
    (*pathIndex)--;
    return 0;
}

static int matrixHasCycle(const IGraph *self, int startVertexId, int *cyclePath, int *cycleLength)
{
    const MatrixGraphImpl *impl;
    int visited[MAX_VERTEX_COUNT];
    int inStack[MAX_VERTEX_COUNT];
    int path[MAX_VERTEX_COUNT];
    int pathIndex = 0;
    int hasCycleFlag = 0;
    int i;

    if (self == NULL || self->implData == NULL) {
        return 0;
    }
    impl = getConstImpl(self);

    if (!isValidVertexId(impl, startVertexId)) {
        return 0;
    }

    memset(visited, 0, sizeof(visited));
    memset(inStack, 0, sizeof(inStack));
    memset(path, -1, sizeof(path));

    cycleRecursive(impl, startVertexId, visited, inStack, path, &pathIndex, &hasCycleFlag);

    if (hasCycleFlag && cyclePath != NULL && cycleLength != NULL) {
        *cycleLength = 0;
        for (i = 0; i < pathIndex; i++) {
            cyclePath[i] = path[i];
            (*cycleLength)++;
        }
    }

    return hasCycleFlag;
}

static int matrixDijkstraShortestPath(const IGraph *self, int startVertexId, int endVertexId, int *path, int *pathLength, int *shortestDistance)
{
    const MatrixGraphImpl *impl;
    int dist[MAX_VERTEX_COUNT];
    int visited[MAX_VERTEX_COUNT];
    int predecessor[MAX_VERTEX_COUNT];
    int count;
    int i, j, k;
    int minDist;
    int u;

    if (self == NULL || self->implData == NULL) {
        return -1;
    }
    impl = getConstImpl(self);
    count = impl->vertexCount;

    if (!isValidVertexId(impl, startVertexId) || !isValidVertexId(impl, endVertexId)) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        dist[i] = INFINITE_WEIGHT;
        visited[i] = 0;
        predecessor[i] = -1;
    }
    dist[startVertexId] = 0;

    for (i = 0; i < count; i++) {
        minDist = INFINITE_WEIGHT;
        u = -1;
        for (j = 0; j < count; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        for (k = 0; k < count; k++) {
            if (!visited[k] &&
                impl->weightMatrix[u][k] != INFINITE_WEIGHT &&
                dist[u] + impl->weightMatrix[u][k] < dist[k]) {
                dist[k] = dist[u] + impl->weightMatrix[u][k];
                predecessor[k] = u;
            }
        }
    }

    if (dist[endVertexId] == INFINITE_WEIGHT) {
        return -1;
    }

    if (shortestDistance != NULL) {
        *shortestDistance = dist[endVertexId];
    }

    if (path != NULL && pathLength != NULL) {
        *pathLength = 0;
        int tempPath[MAX_VERTEX_COUNT];
        int tempIndex = 0;
        int current = endVertexId;

        while (current != -1) {
            tempPath[tempIndex++] = current;
            current = predecessor[current];
        }

        for (i = tempIndex - 1; i >= 0; i--) {
            path[*pathLength] = tempPath[i];
            (*pathLength)++;
        }
    }

    return 0;
}

static const GraphOps sMatrixGraphOps = {
    matrixCreateGraph,
    matrixDestroyGraph,
    matrixAddVertex,
    matrixAddEdge,
    matrixGetVertexCount,
    matrixGetVertexById,
    matrixHasEdge,
    matrixGetEdgeWeight,
    matrixDisplayGraph,
    matrixDisplayVertexList,
    matrixDfsTraverse,
    matrixHasCycle,
    matrixDijkstraShortestPath
};

IGraph *createAdjacencyMatrixGraph(void)
{
    IGraph *instance = (IGraph *)malloc(sizeof(IGraph));
    MatrixGraphImpl *impl = (MatrixGraphImpl *)malloc(sizeof(MatrixGraphImpl));
    if (instance == NULL || impl == NULL) {
        free(instance);
        free(impl);
        return NULL;
    }
    memset(impl, 0, sizeof(*impl));
    initWeightMatrix(impl->weightMatrix);
    instance->ops = &sMatrixGraphOps;
    instance->implData = impl;
    return instance;
}

void releaseGraphInstance(IGraph *graph)
{
    if (graph == NULL) {
        return;
    }
    if (graph->ops && graph->ops->destroyGraph) {
        graph->ops->destroyGraph(graph);
    }
    free(graph->implData);
    free(graph);
}
