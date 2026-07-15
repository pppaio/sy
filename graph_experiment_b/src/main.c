#include <stdio.h>
#include <string.h>
#include "../include/graph_interface.h"
#include "../include/console_utils.h"

static void printMenuBanner(void)
{
    printf("\n========== 景区路径规划系统 [B角色: 邻接矩阵 + SOLID重构版] ==========\n");
    printf("  1. 创建景区图（选择 有向/无向）\n");
    printf("  2. 添加景点\n");
    printf("  3. 添加路径（两景点间道路及其长度）\n");
    printf("  4. 显示景区图（邻接矩阵）\n");
    printf("  5. 显示景点链表\n");
    printf("  6. DFS导游线路生成（从指定景点出发）\n");
    printf("  7. 回路检测（检测导游线路中是否存在回路）\n");
    printf("  8. Dijkstra最短路径查询\n");
    printf("  9. 一键运行内置演示（8景点标准示例）\n");
    printf(" 10. 销毁当前景区图（重置）\n");
    printf(" 11. 查询接口测试(查某景点详情 / 边权重 / 边是否存在)\n");
    printf("  0. 退出系统\n");
    printf("=====================================================================\n");
    printf("请输入选项: ");
}

static int isGraphReady(const IGraph *graph)
{
    return (graph != NULL && graphGetVertexCount(graph) > 0);
}

static void handleCreateGraph(IGraph *graph)
{
    int graphDirection;
    printf("请选择图类型: 0=无向图(默认), 1=有向图: ");
    graphDirection = consoleReadInt(0);
    graphCreate(graph, graphDirection);
    printf("创建成功: %s图, 最大支持 %d 个景点\n",
           graphDirection ? "有向" : "无向", MAX_VERTEX_COUNT);
}

static void handleAddVertex(IGraph *graph)
{
    char vertexName[MAX_VERTEX_NAME_LENGTH];
    char vertexDesc[MAX_VERTEX_DESC_LENGTH];
    int newVertexId;

    printf("当前已有景点 %d 个\n", graphGetVertexCount(graph));
    printf("请输入景点名称: ");
    consoleClearInputBuffer();
    consoleReadLine(vertexName, sizeof(vertexName));
    if (strlen(vertexName) == 0) {
        printf("景点名称不能为空！\n");
        return;
    }
    printf("请输入景点描述(可直接回车跳过): ");
    consoleReadLine(vertexDesc, sizeof(vertexDesc));

    newVertexId = graphAddVertex(
        graph,
        vertexName,
        strlen(vertexDesc) > 0 ? vertexDesc : NULL
    );
    if (newVertexId >= 0) {
        GraphVertex vertexInfo;
        graphGetVertexById(graph, newVertexId, &vertexInfo);
        printf("添加成功: 景点编号(V%d) = %s\n", newVertexId, vertexInfo.vertexName);
    }
}

static void handleAddEdge(IGraph *graph)
{
    int fromVertexId;
    int toVertexId;
    int edgeWeight;
    int vertexCount = graphGetVertexCount(graph);
    GraphVertex fromVertex;
    GraphVertex toVertex;

    if (vertexCount < 2) {
        printf("景点不足(当前 %d 个)，请先添加至少两个景点\n", vertexCount);
        return;
    }

    graphDisplayVertexList(graph);

    printf("请输入起点景点编号 (V?): ");
    fromVertexId = consoleReadInt(-1);
    printf("请输入终点景点编号 (V?): ");
    toVertexId = consoleReadInt(-1);
    printf("请输入路径长度(正整数): ");
    edgeWeight = consoleReadInt(-1);

    if (graphAddEdge(graph, fromVertexId, toVertexId, edgeWeight) == 0) {
        graphGetVertexById(graph, fromVertexId, &fromVertex);
        graphGetVertexById(graph, toVertexId, &toVertex);
        printf("路径添加成功: %s(V%d) --%d--> %s(V%d)\n",
               fromVertex.vertexName, fromVertexId, edgeWeight,
               toVertex.vertexName, toVertexId);
    }
}

static void handleQueryApi(IGraph *graph)
{
    int vertexId;
    int fromId;
    int toId;
    GraphVertex vertexInfo;
    if (!isGraphReady(graph)) {
        printf("请先创建图并添加景点\n");
        return;
    }

    printf("-- 查询顶点详情 --\n");
    printf("请输入顶点编号(V?): ");
    vertexId = consoleReadInt(-1);
    if (graphGetVertexById(graph, vertexId, &vertexInfo) == 0) {
        printf("  顶点查询结果: ");
        consolePrintVertexSummary(&vertexInfo);
        printf("\n");
    } else {
        printf("  顶点编号 %d 不存在\n", vertexId);
    }

    printf("-- 查询边是否存在及权重 --\n");
    printf("请输入起点ID 终点ID（空格分隔）: ");
    fromId = consoleReadInt(-1);
    toId = consoleReadInt(-1);
    if (graphHasEdge(graph, fromId, toId)) {
        printf("  V%d -> V%d 存在直接路径，权重 = %d\n",
               fromId, toId, graphGetEdgeWeight(graph, fromId, toId));
    } else {
        printf("  V%d -> V%d 不存在直接路径(∞)\n", fromId, toId);
    }
}

static void handleDfsTraverse(IGraph *graph)
{
    int startVertexId;
    if (!isGraphReady(graph)) {
        printf("请先创建图并添加景点\n");
        return;
    }
    printf("请输入起始景点编号(V?): ");
    startVertexId = consoleReadInt(-1);
    graphDfsTraverse(graph, startVertexId);
}

static void handleCycleDetection(IGraph *graph)
{
    int startVertexId;
    int cyclePath[MAX_VERTEX_COUNT];
    int cycleLength = 0;
    int i;
    GraphVertex vertexInfo;

    if (!isGraphReady(graph)) {
        printf("请先创建图并添加景点\n");
        return;
    }

    printf("请输入起始景点编号(V?): ");
    startVertexId = consoleReadInt(-1);

    printf("\n==================== 回路检测结果 ====================\n");
    printf("检测起点: V%d\n", startVertexId);

    if (graphHasCycle(graph, startVertexId, cyclePath, &cycleLength)) {
        printf("检测结果: 存在回路！\n");
        printf("回路路径: ");
        for (i = 0; i < cycleLength; i++) {
            graphGetVertexById(graph, cyclePath[i], &vertexInfo);
            printf("V%d(%s)", cyclePath[i], vertexInfo.vertexName);
            if (i < cycleLength - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
    } else {
        printf("检测结果: 未发现回路\n");
    }
    printf("======================================================\n\n");
}

static void handleShortestPath(IGraph *graph)
{
    int startVertexId;
    int endVertexId;
    int path[MAX_VERTEX_COUNT];
    int pathLength = 0;
    int shortestDistance = 0;
    int i;
    GraphVertex vertexInfo;

    if (!isGraphReady(graph)) {
        printf("请先创建图并添加景点\n");
        return;
    }

    printf("请输入起点景点编号(V?): ");
    startVertexId = consoleReadInt(-1);
    printf("请输入终点景点编号(V?): ");
    endVertexId = consoleReadInt(-1);

    printf("\n==================== Dijkstra最短路径查询 ====================\n");
    printf("查询: V%d -> V%d\n", startVertexId, endVertexId);

    if (graphDijkstraShortestPath(graph, startVertexId, endVertexId, path, &pathLength, &shortestDistance) == 0) {
        printf("最短距离: %d 米\n", shortestDistance);
        printf("最短路径: ");
        for (i = 0; i < pathLength; i++) {
            graphGetVertexById(graph, path[i], &vertexInfo);
            printf("V%d(%s)", path[i], vertexInfo.vertexName);
            if (i < pathLength - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
    } else {
        printf("无法到达（两景点间无连通路径）\n");
    }
    printf("==============================================================\n\n");
}

static void loadScenicDemoData(IGraph *graph)
{
    printf(">>> 正在加载【标准景区8景点】演示数据...\n");
    graphCreate(graph, 0);

    graphAddVertex(graph, "大门",     "景区入口");
    graphAddVertex(graph, "碧玉潭",   "清澈碧潭");
    graphAddVertex(graph, "红叶林",   "秋季红叶");
    graphAddVertex(graph, "赏花园",   "四季花卉");
    graphAddVertex(graph, "飞云石",   "奇特山石");
    graphAddVertex(graph, "九曲桥",   "曲折石桥");
    graphAddVertex(graph, "白虎岩",   "天然岩石");
    graphAddVertex(graph, "观月阁",   "赏月佳处");

    graphAddEdge(graph, 0, 1, 100);
    graphAddEdge(graph, 0, 2, 150);
    graphAddEdge(graph, 1, 3, 80);
    graphAddEdge(graph, 2, 4, 120);
    graphAddEdge(graph, 3, 5, 60);
    graphAddEdge(graph, 4, 5, 90);
    graphAddEdge(graph, 5, 6, 70);
    graphAddEdge(graph, 5, 7, 110);
    graphAddEdge(graph, 6, 7, 50);

    printf(">>> 演示数据加载完成: %d 个景点, %d 条路径\n", graphGetVertexCount(graph), 9);
    graphDisplayGraph(graph);
    graphDisplayVertexList(graph);

    printf(">>> 演示DFS导游线路（从大门V0出发）:\n");
    graphDfsTraverse(graph, 0);

    printf(">>> 演示回路检测:\n");
    {
        int cyclePath[MAX_VERTEX_COUNT];
        int cycleLength = 0;
        if (graphHasCycle(graph, 0, cyclePath, &cycleLength)) {
            printf("  检测结果: 存在回路\n");
        } else {
            printf("  检测结果: 无回路\n");
        }
    }

    printf(">>> 演示Dijkstra最短路径（大门V0 -> 观月阁V7）:\n");
    {
        int path[MAX_VERTEX_COUNT];
        int pathLength = 0;
        int distance = 0;
        if (graphDijkstraShortestPath(graph, 0, 7, path, &pathLength, &distance) == 0) {
            printf("  最短距离: %d米\n", distance);
        }
    }
}

int main(void)
{
    IGraph *scenicGraph;
    int userChoice;
    int graphInitialized = 0;

    consoleInitEncoding();
    scenicGraph = createAdjacencyMatrixGraph();

    if (scenicGraph == NULL) {
        printf("系统初始化失败: 无法创建景区图实例\n");
        return 1;
    }

    do {
        printMenuBanner();
        userChoice = consoleReadInt(-1);

        switch (userChoice) {
        case 1:
            handleCreateGraph(scenicGraph);
            graphInitialized = 1;
            break;
        case 2:
            if (!graphInitialized) {
                printf("请先选择 [1] 创建景区图\n");
                break;
            }
            handleAddVertex(scenicGraph);
            break;
        case 3:
            if (!graphInitialized) {
                printf("请先选择 [1] 创建景区图\n");
                break;
            }
            handleAddEdge(scenicGraph);
            break;
        case 4:
            if (!isGraphReady(scenicGraph)) {
                printf("请先创建图并添加景点\n");
                break;
            }
            graphDisplayGraph(scenicGraph);
            break;
        case 5:
            if (!isGraphReady(scenicGraph)) {
                printf("请先创建图并添加景点\n");
                break;
            }
            graphDisplayVertexList(scenicGraph);
            break;
        case 6:
            if (!isGraphReady(scenicGraph)) {
                printf("请先创建图并添加景点\n");
                break;
            }
            handleDfsTraverse(scenicGraph);
            break;
        case 7:
            if (!isGraphReady(scenicGraph)) {
                printf("请先创建图并添加景点\n");
                break;
            }
            handleCycleDetection(scenicGraph);
            break;
        case 8:
            if (!isGraphReady(scenicGraph)) {
                printf("请先创建图并添加景点\n");
                break;
            }
            handleShortestPath(scenicGraph);
            break;
        case 9:
            loadScenicDemoData(scenicGraph);
            graphInitialized = 1;
            break;
        case 10:
            graphDestroy(scenicGraph);
            printf("景区图已销毁重置\n");
            graphInitialized = 0;
            break;
        case 11:
            handleQueryApi(scenicGraph);
            break;
        case 0:
            printf("感谢使用景区路径规划系统，再见！\n");
            break;
        default:
            printf("无效选项(0~11), 请重试\n");
            break;
        }
    } while (userChoice != 0);

    releaseGraphInstance(scenicGraph);
    return 0;
}
