================================================================
B角色：邻接矩阵存储结构图实现 - 解题思路文档
            （景区路径规划系统）
================================================================

一、题目梳理与设计思路
----------------------------------------------------------------
目标：基于【二维数组 + 结构体组合】，用邻接矩阵(Adjacency Matrix)
     实现景区路径规划系统的"图"核心模块。

必做5项功能：
    [1] 创建图      CreateGraph()
    [2] 添加景点    AddVertex()
    [3] 添加路径    AddEdge()
    [4] 显示景区图  DisplayGraph()      ：打印矩阵表格
    [5] 显示景点链表 DisplayVertexList() ：以链表视图遍历打印各顶点

数据结构选型（邻接矩阵 vs 邻接表 适用场景）：
    ┌───────────────┬──────────────────────┬──────────────────────┐
    │   对比维度      │   邻接矩阵（B角色）    │   邻接表（A角色）     │
    ├───────────────┼──────────────────────┼──────────────────────┤
    │  存储方式       │ 二维数组紧凑存储       │ 指针链表动态存储      │
    │  空间复杂度     │ O(V²)                 │ O(V+E)               │
    │  适合的图       │ 稠密图(E接近V²)        │ 稀疏图(E<<V²)        │
    │  判(u,v)边      │ O(1)直接下标           │ O(degree(u))         │
    │  新增/删除顶点  │ 较麻烦（扩容）         │ 灵活，链表插删       │
    │  本次实验场景   │ 景区景点数少(≤20)       │ ——                  │
    └───────────────┴──────────────────────┴──────────────────────┘
    → 景区通常景点数有限(<20)，选矩阵：判路O(1) + 打印直观。

整体程序流程图：
          ┌──────────────────┐
          │  Start (main.c)   │
          └────────┬─────────┘
                   v
          ┌──────────────────┐
          │  打印主菜单循环    │ ←──┐
          └────────┬─────────┘    │
          ┌────────┴────────────┐│
          │ 1建图 2加景点 3加路径││
          │ 4显示矩阵 5显示链表 ││
          │ 6演示 7销毁 0退出   ││
          └────────┬────────────┘│
         ┌─────────┼─────────────┘
         v         v           v
    CreateGraph AddVertex  AddEdge
         │         │           │
         └─────────┴───────────┘
                   v
          MGraph结构体（被维护）
         ┌────── vertices[0..n-1]
         │        └─ id/name/desc
         └────── edgeMatrix[n×n]
                  └─ weight or INF

二、工程目录结构图（C工程化规范）
----------------------------------------------------------------

graph_experiment_b/            （B角色 工作根目录）
├── include/                    【头文件目录（对外接口）】
│   └── graph_matrix.h        5个API声明 + Vertex/MGraph结构体
├── src/                        【实现文件目录（私有逻辑）】
│   ├── graph_matrix.c        5个API实现 + InitMatrix静态内部函数
│   └── main.c                主菜单交互（6选项+演示+退出）
├── build/                      【编译输出目录（.exe/.o）】
│   └── scenic_graph_matrix.exe（CMake生成 / 或gcc生成）
├── data/                       【测试数据目录】
│   └── scenic_input_demo.txt （备用外部测试用例）
├── docs/                       【文档目录】
│   └── solution_b.md         本文档（伪代码/流程图/目录树）
├── CMakeLists.txt              【CMake自动化构建脚本】
└── README.md                   （可选：使用说明+构建命令）

三、伪代码实现（5个核心函数）
----------------------------------------------------------------

【结构体定义】
    Vertex { int id; char name[32]; char desc[64]; }
    MGraph {
        Vertex vertices[MAX_VERTICES];
        int    edgeMatrix[MAX_VERTICES][MAX_VERTICES]; // 0/INF/权值
        int    vertexCount;
        int    directed;    // 0=无向，1=有向
    }

----------------------------------------------------------------
Function CreateGraph(MGraph *g, int directed)
    // 功能：初始化一张空图
    g.vertexCount ← 0
    g.directed    ← directed
    FOR i FROM 0 TO MAX_VERTICES-1:
        FOR j FROM 0 TO MAX_VERTICES-1:
            IF i == j:  g.edgeMatrix[i][j] ← 0   // 自己到自己
            ELSE:        g.edgeMatrix[i][j] ← INF // 无通路
        END FOR
    END FOR
    memset(g.vertices, 0, sizeof(g.vertices))
End Function

----------------------------------------------------------------
Function AddVertex(MGraph *g, name, desc) → int 返回新顶点ID
    // 功能：新增一个景点顶点
    IF g.vertexCount >= MAX_VERTICES: RETURN -1 (失败)
    idx ← g.vertexCount
    g.vertices[idx].id   ← idx
    g.vertices[idx].name ← name   (strncpy防越界)
    g.vertices[idx].desc ← desc   (desc可为空串)
    g.vertexCount ← g.vertexCount + 1
    RETURN idx
End Function

----------------------------------------------------------------
Function AddEdge(MGraph *g, fromId, toId, weight) → int
    // 功能：新增一条路径（边）from→to，权值weight
    IF fromId/toId越界 OR weight<0: RETURN -1
    g.edgeMatrix[fromId][toId] ← weight
    IF g.directed == 0:          // 无向图：对称
        g.edgeMatrix[toId][fromId] ← weight
    END IF
    RETURN 0
End Function

----------------------------------------------------------------
Function DisplayGraph(const MGraph *g)
    // 功能：以矩阵表格形式打印景区路径图
    PRINT 标题 / 图类型 / 顶点数
    // 表头行：打印所有顶点名缩写
    FOR j TO vertexCount-1: PRINT "Vj:name"
    PRINT 换行
    // 矩阵主体
    FOR i TO vertexCount-1:
        PRINT "Vi:name"
        FOR j TO vertexCount-1:
            w ← g.edgeMatrix[i][j]
            IF w == INF: PRINT "INF"
            ELSE: PRINT w
        END FOR
        PRINT 换行
    END FOR
End Function

----------------------------------------------------------------
Function DisplayVertexList(const MGraph *g)
    // 功能：以"景点链表视图"打印每个顶点及其邻居
    //       (使矩阵实现也能看到邻接关系链)
    PRINT 表头：[HEAD]--->[V0:xx]--->[V1:yy]...--->[NULL]
    FOR i TO vertexCount-1:
        PRINT "顶点链表#i | Vi:name (desc)"
        PRINT "相邻: "
        first ← true
        FOR j TO vertexCount-1:
            IF i≠j AND g.edgeMatrix[i][j]≠INF:
                IF NOT first: PRINT " -> "
                PRINT "[Vj:name | weight]"
                first ← false
            END IF
        END FOR
        IF first: PRINT "(无相邻景点)"
        PRINT 换行
    END FOR
End Function

四、测试数据设计（演示用：湖光岩景区）
----------------------------------------------------------------
共 7 个顶点：
  V0 西门入口    V1 玛珥湖广场  V2 火山博物馆
  V3 望海楼      V4 楞严寺      V5 九曲桥      V6 东门出口

共 10 条无向路径（权值=米）：
  V0-V1(500)  V0-V2(300)  V1-V2(200)  V1-V3(450)  V1-V5(150)
  V2-V4(350)  V3-V6(700)  V4-V5(220)  V5-V6(400)  V3-V5(180)

对应邻接矩阵预期（只列i≠j非INF项）：
  row0 : [  -, 500, 300,  -,  -,  -,  - ]
  row1 : [500,  -, 200, 450,  -, 150,  - ]
  row2 : [300, 200,  -,  -, 350,  -,  - ]
  row3 : [  -, 450,  -,  -,  -, 180, 700]
  row4 : [  -,  -, 350,  -,  -, 220,  - ]
  row5 : [  -, 150,  -, 180, 220,  -, 400]
  row6 : [  -,  -,  -, 700,  -, 400,  - ]
