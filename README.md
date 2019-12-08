# GameOfTheAmazons
用极大极小搜索和AlphaBeta剪枝下亚马逊棋

#### 软件各源文件的主要内容和功能：

**Amazons_recover.cpp**  控制台程序的入口

**std.h**  为了方便在msvc下编译，照抄g++ <stdc++.h>中的内容；并定义通用的预编译宏

**csbd.h**  位棋盘数据结构和位棋盘上的操作，定义保存棋盘状态的结构

**game.h**  定义保存棋局状态的数据结构（包括目前移动的颜色和轮次），并定义了record_table类型，用来保存搜索过程中产生的数据

**eval.h**  评估函数入口，初始化评估引擎

**eval_head.h**  定义评估过程中所要用到的数据结构，这是为了兼容csbd.h中的结构

**container.h**  用变长数组实现了一个queue类容器，用于kmove和qmove的计算

**eval_main.h**  评估部分的主体，含有评估加权器和评估器

**sio.h**  实现简单输入输出函数

**searcher.h**  实现带pvs的极大极小搜索
