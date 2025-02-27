/*
    问题描述：一个工程的完工需要若干子工程配合完成，每个子工程有自己的计划完工用时，子工程之间存在先后关系；
根据子工程的先后关系建立数据模型，先判断模型的可实施性（是否存在环）, 再计算总工程完工所需时长，并给出关键子工程。
    基本要求：
(1)建立网络模型，要求结点（子工程）数量不低于10个，边（子工程的先后关系）数不低于20条，采用邻接矩阵或邻接表结构存储网；
(2)输出一个拓扑序列；
(3)输出关键路径长度、关键活动、所有关键路径。
*/

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>  // INT_MAX
#define _USE_MATH_DEFINES
#include <math.h>    // M_PI
#include "graphics.h"  // EasyX 图形库

using namespace std;

bool is_any_in_vector(const vector<int>& vec, int element);

struct SubProject {
    int id;         // 结点id
    int duration;   // 持续时间
    vector<int> next;  // 后继
};

// 拓扑排序失败，有向图中存在环
bool ExistCycle(const vector<vector<int>>& graph, int n) {
    // 计算入度
    vector<int> inDegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            // u -> v
            ++inDegree[v];
        }
    }

    // 入度为0的节点即源点入队
    queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    // 尝试拓扑排序
    int visited = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        ++visited;
        for (int v : graph[u]) {
            if (--inDegree[v] == 0) {
                q.push(v);
            }
        }
    }
    return visited != n;
}

// 拓扑排序存入vector
vector<int> topologicalSort(const vector<vector<int>>& graph, int n) {
    // 计算入度
    vector<int> inDegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            ++inDegree[v];
        }
    }

    // 入度为0的节点入队
    queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    // 拓扑排序 BFS + 队列
    vector<int> topoOrder;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        topoOrder.push_back(u);

        for (int v : graph[u]) {
            if (--inDegree[v] == 0) {
                q.push(v);
            }
        }
    }
    return topoOrder;
}

// 关键路径计算
struct CriticalPathResult {
    vector<int> criticalPath;
    int criticalPathLength;
};

CriticalPathResult findCriticalPath(const vector<SubProject>& subproject, const vector<int>& topoOrder) {
    int n = subproject.size();
    vector<int> earliest(n, 0);
    vector<int> latest(n, INT_MAX);

    // 计算最早开始时间 = 最早时间（earliest[u] + subproject[u].duration）与当前 earliest[v] 的较大值
    for (int u : topoOrder) {
        for (int v : subproject[u].next) {
            earliest[v] = max(earliest[v], earliest[u] + subproject[u].duration);
        }
    }

    // 计算最晚开始时间 = 最晚时间（latest[v] - subproject[u].duration）与当前 latest[u] 的较小值
    latest[topoOrder.back()] = earliest[topoOrder.back()];
    // 反向迭代器逆序遍历拓扑排序
    for (auto it = topoOrder.rbegin(); it != topoOrder.rend(); ++it) {
        int u = *it;    
        for (int v : subproject[u].next) {
            latest[u] = min(latest[u], latest[v] - subproject[u].duration);
        }
    }

    // 收集关键路径
    CriticalPathResult result;
    for (int i = 0; i < n; ++i) {
        if (earliest[i] == latest[i]) {
            result.criticalPath.push_back(i);
        }
    }
    result.criticalPathLength = earliest[topoOrder.back()];
    return result;
}

// 节点位置结构体
struct NodePosition {
    int x;  // 修改为整数类型
    int y;
};

// 绘制带箭头的线条
void drawArrowLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness = 1, double arrowSize = 10, double arrowAngle = 30) {
    // 设置线条颜色和样式
    setlinecolor(color);
    setlinestyle(PS_SOLID, thickness);

    // 绘制主线条
    line(x1, y1, x2, y2);

    // 计算线条角度
    double angle = atan2((double)(y2 - y1), (double)(x2 - x1));

    // 计算箭头两边的点
    double angle1 = angle + (M_PI / 180) * arrowAngle;
    double angle2 = angle - (M_PI / 180) * arrowAngle;

    int arrowX1 = (int)(x2 - arrowSize * cos(angle1));
    int arrowY1 = (int)(y2 - arrowSize * sin(angle1));

    int arrowX2 = (int)(x2 - arrowSize * cos(angle2));
    int arrowY2 = (int)(y2 - arrowSize * sin(angle2));

    // 绘制箭头两边
    line(x2, y2, arrowX1, arrowY1);
    line(x2, y2, arrowX2, arrowY2);
}

// 绘制拓扑图和关键路径
void drawGraph(const vector<SubProject>& subproject, const vector<int>& topoOrder, const vector<int>& criticalPath, const vector<NodePosition>& positions) {
    // 绘制边
    for (const auto& proj : subproject) {
        for (int to : proj.next) {
            // 检查是否在关键路径
            bool isCP = false;
            for (size_t i = 0; i < criticalPath.size() - 1; ++i) {
                if (criticalPath[i] == proj.id && criticalPath[i + 1] == to) {
                    isCP = true;
                    break;
                }
            }
            // 设置颜色
            COLORREF edgeColor = isCP ? RED : BLACK;
            int thickness = isCP ? 2 : 1;
            // 绘制线条
            drawArrowLine(positions[proj.id].x, positions[proj.id].y, positions[to].x, positions[to].y, edgeColor, thickness);

            // 计算线条中点位置
            int midX = (positions[proj.id].x + positions[to].x) / 2;
            int midY = (positions[proj.id].y + positions[to].y) / 2;
            
            // 计算线条角度
            double angle = atan2((double)(positions[to].y - positions[proj.id].y), (double)(positions[to].x - positions[proj.id].x));
            
            // 设置标签偏移量，以避免与线条重叠
            int offset = 0; // 调整偏移量大小
            int labelX = midX + static_cast<int>(offset * (-sin(angle)));
            int labelY = midY + static_cast<int>(offset * cos(angle));
            
            // 准备持续时间标签
            TCHAR durLabel[10];
            wsprintf(durLabel, TEXT("%d"), subproject[proj.id].duration);
            
            // 设置文本颜色和样式
            settextcolor(BLUE); // 可以根据需要调整颜色
            settextstyle(12, 0, _T("Consolas")); // 设置文本字体和大小
            
            // 绘制持续时间标签
            outtextxy(labelX, labelY, durLabel);
        }
    }

    // 绘制节点标签
    for (size_t i = 0; i < subproject.size(); ++i) {
        // 设置文本
        settextcolor(is_any_in_vector(criticalPath, subproject[i].id) ? RED : BLACK);
        settextstyle(28, 0, _T("Consolas")); // 设置文本字体和大小
        // 设置文本格式
        TCHAR buffer[10];
        wsprintf(buffer, TEXT("%d"), subproject[i].id);
        // 设置文本对齐方式
        setbkmode(TRANSPARENT);
        // 绘制节点ID，位于节点位置的中心
        outtextxy(positions[i].x - 5, positions[i].y - 7, buffer);
    }
}

// 检查元素是否在vector中
bool is_any_in_vector(const vector<int>& vec, int element) {
    return find(vec.begin(), vec.end(), element) != vec.end();
}

int main() {
    int n, m;
    cout << "请输入子工程数量（>=10）：";
    cin >> n;
    if (n < 10) {
        cout << "子工程数量必须不少于10。" << endl;
        return 1;
    }

    cout << "请输入依赖关系数量（>=20）：";
    cin >> m;
    if (m < 20) {
        cout << "依赖关系数量必须不少于20。" << endl;
        return 1;
    }

    // 子工程
    vector<SubProject> subproject(n);

    // 邻接表
    /*
    邻接表用vector<vector<int>> graph存储，
    graph[u]存储结点u的后继结点，是数组形式，存储结点的id
    */
    vector<vector<int>> graph(n);

    // 输入每个子工程的持续时间
    for (int i = 0; i < n; ++i) {
        subproject[i].id = i;
        cout << "请输入子工程 " << i << " 的持续时间：";
        cin >> subproject[i].duration;
        if (subproject[i].duration <= 0) {
            cout << "持续时间必须为正整数。" << endl;
            --i;  // 重新输入当前子工程
            continue;
        }
    }

    // 输入依赖关系
    cout << "请输入依赖关系（格式：from to）：" << endl;
    for (int i = 0; i < m; ++i) {
        int from, to;
        cin >> from >> to;
        if (from < 0 || from >= n || to < 0 || to >= n) {
            cout << "无效的依赖关系: " << from << " -> " << to << ". 请重新输入。" << endl;
            --i;  // 重新输入当前边
            continue;
        }
        if (from == to) {
            cout << "自环依赖关系无效: " << from << " -> " << to << ". 请重新输入。" << endl;
            --i;  // 重新输入当前边
            continue;
        }
        // 检查是否已经存在相同的依赖关系
        if (find(graph[from].begin(), graph[from].end(), to) != graph[from].end()) {
            cout << "依赖关系 " << from << " -> " << to << " 已存在。请重新输入。" << endl;
            --i;  // 重新输入当前边
            continue;
        }
        subproject[from].next.push_back(to);
        graph[from].push_back(to);
    }

    // 判断是否有环
    if (ExistCycle(graph, n)) {
        cout << "项目存在循环依赖，无法完成。" << endl;
        return 1;
    }

    // 拓扑排序
    vector<int> topoOrder = topologicalSort(graph, n);
    cout << "拓扑排序序列：";
    for (int id : topoOrder) {
        cout << id << " ";
    }
    cout << endl;

    // 计算关键路径
    CriticalPathResult cpResult = findCriticalPath(subproject, topoOrder);
    cout << "关键路径：";
    for (int id : cpResult.criticalPath) {
        cout << id << " ";
    }
    cout << endl;
    cout << "关键路径长度：" << cpResult.criticalPathLength << endl;

    // 初始化 EasyX 图形窗口
    initgraph(800, 600);  // 设置窗口大小
    setbkcolor(WHITE);    // 设置背景颜色为白色
    cleardevice();        // 清空设备

    // 计算节点位置（环形布局）确保所有节点在圆周上均匀分布
    /*
    根据图形窗口的宽度和高度（800x600），选择较小的一边（600）的一半为基础。
    减去100以留出边距，避免节点靠近窗口边缘
    */
    vector<NodePosition> positions(n);
    double angleStep = 2 * M_PI / n;
    double radius = min(800.0, 600.0) / 2 - 100;  // 圆的半径
    double centerX = 400.0;  // 圆心X
    double centerY = 300.0;  // 圆心Y
    for (int i = 0; i < n; ++i) {
        double angle = i * angleStep - M_PI / 2;  // 从顶部开始
        positions[i].x = static_cast<int>(centerX + radius * cos(angle));
        positions[i].y = static_cast<int>(centerY + radius * sin(angle));
    }

    // 绘制图形
    drawGraph(subproject, topoOrder, cpResult.criticalPath, positions);

    // 等待用户关闭窗口
    cout << "图形已绘制。" << endl;
    system("pause");  // 等待用户按键
    closegraph();

    return 0;
}