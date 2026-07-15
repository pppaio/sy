================================================================
B角色：邻接矩阵景区路径规划系统 | v2 SOLID+设计模式重构版
                  Git 协作开发规范 总览
================================================================

一、项目代码设计规范
----------------------------------------------------------------

1. 命名约定 —— 驼峰命名（CamelCase）
   ┌───────────────────────┬────────────────────────────────────┐
   │ 类别                   │ 规则 / 示例                          │
   ├───────────────────────┼────────────────────────────────────┤
   │ 类型 / 结构体          │ 大驼峰 PascalCase                    │
   │                        │ GraphVertex  MatrixGraphImpl  IGraph│
   │                        │ GraphOps                            │
   ├───────────────────────┼────────────────────────────────────┤
   │ 函数 / 变量 / 参数     │ 小驼峰 camelCase                     │
   │                        │ graphCreate()  addVertex()          │
   │                        │ vertexName  fromVertexId  edgeWeight│
   ├───────────────────────┼────────────────────────────────────┤
   │ 宏常量                 │ 全大写下划线分隔（C标准宏约定不改）    │
   │                        │ MAX_VERTEX_COUNT  INFINITE_WEIGHT   │
   ├───────────────────────┼────────────────────────────────────┤
   │ 指针 / 句柄            │ 语义化英文单词，禁止拼音/缩写       │
   │                        │ scenicGraph  vertexInfo  impl       │
   └───────────────────────┴────────────────────────────────────┘
   * 若不知是否"驼峰"正确 → Google 该英文标准命名，参考业界主流项目

2. 设计原则（SOLID + 设计模式）
   ┌──────────────────────────────────────────────────────────────┐
   │ SRP 单一职责原则：每个模块/函数只做一件事                     │
   │   console_utils 只管输入输出辅助，graph_matrix 只处理数据    │
   │   graph_interface 只定义接口，main 只做高层流程编排          │
   │                                                              │
   │ OCP 开闭原则：对扩展开放，对修改关闭                         │
   │   新图存储实现（如邻接表/十字链表）只需：                    │
   │   新建 graph_list.c → 实现 GraphOps 虚表 → main零修改        │
   │                                                              │
   │ LSP 里氏替换原则：GraphOps 任何实现可互换                    │
   │   同一份 graphCreate()/graphDisplayGraph() 不用改就能切换    │
   │                                                              │
   │ ISP 接口隔离原则：接口拆分为最小粒度，避免"胖接口"           │
   │   读接口: getVertexCount/getVertexById/hasEdge              │
   │   写接口: addVertex/addEdge/createGraph/destroyGraph        │
   │   展示接口: displayGraph/displayVertexList                  │
   │   可按需扩展 shortestPath / dfs / bfs 虚函数                │
   │                                                              │
   │ DIP 依赖倒置原则：依赖抽象不依赖具体                         │
   │   main.c 只 #include graph_interface.h                      │
   │   绝不直接引用 MatrixGraphImpl                               │
   │   通过 createAdjacencyMatrixGraph() 工厂注入具体实现        │
   │                                                              │
   │ 设计模式应用：                                               │
   │   ① 策略模式 Strategy：GraphOps 虚函数表 = 策略接口         │
   │      邻接矩阵策略/邻接表策略 可运行时互换                   │
   │   ② 工厂模式 Factory：createAdjacencyMatrixGraph() 工厂     │
   │      封装对象创建细节，返回抽象 IGraph*                     │
   │   ③ 模板方法：main 编排流程 handleCreate/Add/... 可复用      │
   └──────────────────────────────────────────────────────────────┘

3. 编码质量最佳实践（C语言）
   - 所有指针形参先判空（NULL检查），避免野指针
   - strncpy + 末尾手动置 '\0'，防越界（safeStringCopy）
   - scanf 返回值必检查，失败则清空缓冲防死循环
   - malloc 结果判空，失败要先 free 已分配资源再返回
   - releaseGraphInstance() 配对释放 implData 与 instance，避免内存泄漏

二、Git 分支策略（严格执行以下流程）
----------------------------------------------------------------

分支层次图（3层）：

    master (主分支，只接收合并，禁止直接提交)
       ▲  merge 合并请求（Pull/Merge Request）
       │
    feature (功能主分支，项目总功能分支，直接在此基础切个人分支)
       ▲  merge 合并完成的 feature_xxx
       │
    feature_xxx  (个人功能分支，xxx=姓名拼音缩写，如 lsg/xzg/zhangsan)
       │
       └──→ 每次开发前先：git pull origin feature → 同步最新
       └──→ 开发完本地功能 → commit → push origin feature_xxx
       └──→ 提 merge request：  feature_xxx  →  feature

创建个人分支示例（B角色：假设姓名缩写 XYG）：

  git init                                           # 仓库未初始化时
  git add -A && git commit -m "chore: 初始化项目骨架"
  git branch -M master                                # 主分支名master
  git checkout -b feature                             # 创建项目功能总分支
  git push -u origin feature                          # 推送feature
  git checkout -b feature_xyg                         # 从feature切出个人分支
  git push -u origin feature_xyg                      # 推送个人分支

开发前同步 feature 分支（每天开工 / 每次push前必须执行）：

  git checkout feature_xyg                           # 切回自己的分支
  git fetch origin                                    # 拉取远端最新
  git merge origin/feature                            # 把feature更新合并进来
  (若有冲突 → 手动解决 → git add 冲突文件 → git commit)

开发完成 → 合并回 feature：

  git add graph_experiment_b/*
  git commit -m "feature: 邻接矩阵5项核心API + 查询接口扩展"
  git push origin feature_xyg
  (Gitee/GitHub网页创建 Pull Request: feature_xyg → feature)

feature 全部功能A+B就绪后 → 再合并到 master

  git checkout master
  git merge feature -m "release: 合并A+B图实验功能 → master"
  git push origin master

三、Commit 提交规范
----------------------------------------------------------------

每完成一个完整功能点提交一次 commit，禁止一次几百个文件的大提交。
格式：  <type>: <中文/英文简短描述>

type 取值如下：

  feature:   新增功能     feature: 邻接矩阵5项API(createGraph/addVertex...)
  fix:       修复Bug      fix: 修复AddEdge负数权值未拒绝问题
  perf:      性能优化     perf: 矩阵初始化由memset改为显式双循环置INF
  refactor:  代码重构不改变行为 refactor: 抽取GraphOps虚函数表(SOLID重构)
  docs:      文档变更     docs: 更新README分支策略+提交规范章节
  style:     格式/命名    style: 全局变量与函数统一改为驼峰命名
  test:      测试相关     test: 补充湖光岩7景点10路径演示用例
  chore:     构建/工具链  chore: CMake脚本新增 console_utils.c
  merge:     合并分支     merge: 合并feature_lsg邻接表分支 → feature

示例 commit 序列（B角色一次完整开发）：

  chore: 搭建邻接矩阵工程目录骨架 + CMakeLists
  feature: 新增graph_interface抽象层与GraphOps策略模式虚表
  feature: 邻接矩阵实现createGraph/destroyGraph
  feature: 邻接矩阵实现addVertex/addEdge并含越界/负值校验
  feature: 邻接矩阵实现displayGraph/displayVertexList显示
  feature: 主流程main声明式编排交互菜单 + 湖光岩演示
  style: 全部标识符改为驼峰命名(vertexId/edgeWeight/...)
  perf: safeStringCopy封装strncpy防越界
  docs: 补充README分支/提交规范 + SOLID设计原则说明
  test: 新增查询接口8号菜单项(getVertexById/hasEdge)

================================================================
构建命令（中文乱码必看！Windows下源码UTF-8，控制台默认GBK）
================================================================

【gcc —— 强烈推荐：方案A（转GBK字符串，最稳，双击exe也正常）】
  cd graph_experiment_b
  gcc -I./include -Wall -Wextra -std=c11 ^
      -finput-charset=UTF-8 -fexec-charset=GBK ^
      -o build/scenic_graph_matrix ^
      src/graph_matrix.c src/console_utils.c src/main.c
  .\build\scenic_graph_matrix.exe

  说明: -finput-charset=UTF-8   → 告诉GCC源文件是UTF-8编码
        -fexec-charset=GBK      → 编译时把字符串字面量转换为GBK
        这样即使是双击打开exe(Windows默认cmd代码页936=GBK)也不中文

【gcc —— 方案B（运行时切控制台UTF-8代码页65001）】
  cd graph_experiment_b
  gcc -I./include -Wall -Wextra -std=c11 ^
      -o build/scenic_graph_matrix ^
      src/graph_matrix.c src/console_utils.c src/main.c
  说明: 程序启动时 consoleInitEncoding() 会自动执行:
        SetConsoleOutputCP(65001) + chcp 65001
        适用于Windows Terminal/VSCode等支持UTF-8的终端

【CMake】
  cd graph_experiment_b
  cmake -S . -B build
        (若要GBK编译，在CMakeLists.txt里加 add_compile_definitions / 或
         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=GBK")
  cmake --build build --config Release
  .\build\scenic_graph_matrix.exe
