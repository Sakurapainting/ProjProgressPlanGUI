/*
    ����������һ�����̵��깤��Ҫ�����ӹ��������ɣ�ÿ���ӹ������Լ��ļƻ��깤��ʱ���ӹ���֮������Ⱥ��ϵ��
�����ӹ��̵��Ⱥ��ϵ��������ģ�ͣ����ж�ģ�͵Ŀ�ʵʩ�ԣ��Ƿ���ڻ���, �ټ����ܹ����깤����ʱ�����������ؼ��ӹ��̡�
    ����Ҫ��
(1)��������ģ�ͣ�Ҫ���㣨�ӹ��̣�����������10�����ߣ��ӹ��̵��Ⱥ��ϵ����������20���������ڽӾ�����ڽӱ�ṹ�洢����
(2)���һ���������У�
(3)����ؼ�·�����ȡ��ؼ�������йؼ�·����
*/

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>  // INT_MAX
#define _USE_MATH_DEFINES
#include <math.h>    // M_PI
#include "graphics.h"  // EasyX ͼ�ο�

using namespace std;

bool is_any_in_vector(const vector<int>& vec, int element);

struct SubProject {
    int id;         // ���id
    int duration;   // ����ʱ��
    vector<int> next;  // ���
};

// ��������ʧ�ܣ�����ͼ�д��ڻ�
bool ExistCycle(const vector<vector<int>>& graph, int n) {
    // �������
    vector<int> inDegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            // u -> v
            ++inDegree[v];
        }
    }

    // ���Ϊ0�Ľڵ㼴Դ�����
    queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    // ������������
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

// �����������vector
vector<int> topologicalSort(const vector<vector<int>>& graph, int n) {
    // �������
    vector<int> inDegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            ++inDegree[v];
        }
    }

    // ���Ϊ0�Ľڵ����
    queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    // �������� BFS + ����
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

// �ؼ�·������
struct CriticalPathResult {
    vector<int> criticalPath;
    int criticalPathLength;
};

CriticalPathResult findCriticalPath(const vector<SubProject>& subproject, const vector<int>& topoOrder) {
    int n = subproject.size();
    vector<int> earliest(n, 0);
    vector<int> latest(n, INT_MAX);

    // �������翪ʼʱ�� = �������ʱ�䣨earliest[u] + subproject[u].duration���뵱ǰ earliest[v] �Ľϴ�ֵ
    for (int u : topoOrder) {
        for (int v : subproject[u].next) {
            earliest[v] = max(earliest[v], earliest[u] + subproject[u].duration);
        }
    }

    // ��������ʼʱ�� = �������ʱ�䣨latest[v] - subproject[u].duration���뵱ǰ latest[u] �Ľ�Сֵ
    latest[topoOrder.back()] = earliest[topoOrder.back()];
    // ������������������������
    for (auto it = topoOrder.rbegin(); it != topoOrder.rend(); ++it) {
        int u = *it;    // ������
        for (int v : subproject[u].next) {
            latest[u] = min(latest[u], latest[v] - subproject[u].duration);
        }
    }

    // �ռ��ؼ�·��
    CriticalPathResult result;
    for (int i = 0; i < n; ++i) {
        if (earliest[i] == latest[i]) {
            result.criticalPath.push_back(i);
        }
    }
    result.criticalPathLength = earliest[topoOrder.back()];
    return result;
}

// �ڵ�λ�ýṹ��
struct NodePosition {
    int x;  // �޸�Ϊ��������
    int y;
};

// ���ƴ���ͷ������
void drawArrowLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness = 1, double arrowSize = 10, double arrowAngle = 30) {
    // ����������ɫ����ʽ
    setlinecolor(color);
    setlinestyle(PS_SOLID, thickness);

    // ����������
    line(x1, y1, x2, y2);

    // ���������Ƕ�
    double angle = atan2((double)(y2 - y1), (double)(x2 - x1));

    // �����ͷ���ߵĵ�
    double angle1 = angle + (M_PI / 180) * arrowAngle;
    double angle2 = angle - (M_PI / 180) * arrowAngle;

    int arrowX1 = (int)(x2 - arrowSize * cos(angle1));
    int arrowY1 = (int)(y2 - arrowSize * sin(angle1));

    int arrowX2 = (int)(x2 - arrowSize * cos(angle2));
    int arrowY2 = (int)(y2 - arrowSize * sin(angle2));

    // ���Ƽ�ͷ����
    line(x2, y2, arrowX1, arrowY1);
    line(x2, y2, arrowX2, arrowY2);
}

// ��������ͼ�͹ؼ�·���������Ʊߺͽڵ��ǩ��������Բ��
void drawGraph(const vector<SubProject>& subproject, const vector<int>& topoOrder, const vector<int>& criticalPath, const vector<NodePosition>& positions) {
    // ���Ʊ�
    for (const auto& proj : subproject) {
        for (int to : proj.next) {
            // ����Ƿ��ڹؼ�·��
            bool isCP = false;
            for (size_t i = 0; i < criticalPath.size() - 1; ++i) {
                if (criticalPath[i] == proj.id && criticalPath[i + 1] == to) {
                    isCP = true;
                    break;
                }
            }
            // ������ɫ
            COLORREF edgeColor = isCP ? RED : BLACK;
            int thickness = isCP ? 2 : 1;
            // ��������
            drawArrowLine(positions[proj.id].x, positions[proj.id].y, positions[to].x, positions[to].y, edgeColor, thickness);

            // ���������е�λ��
            int midX = (positions[proj.id].x + positions[to].x) / 2;
            int midY = (positions[proj.id].y + positions[to].y) / 2;
            
            // ���������Ƕ�
            double angle = atan2((double)(positions[to].y - positions[proj.id].y), (double)(positions[to].x - positions[proj.id].x));
            
            // ���ñ�ǩƫ�������Ա����������ص�
            int offset = 0; // ����ƫ������С
            int labelX = midX + static_cast<int>(offset * (-sin(angle)));
            int labelY = midY + static_cast<int>(offset * cos(angle));
            
            // ׼������ʱ���ǩ
            TCHAR durLabel[10];
            wsprintf(durLabel, TEXT("%d"), subproject[proj.id].duration);
            
            // �����ı���ɫ����ʽ
            settextcolor(BLUE); // ���Ը�����Ҫ������ɫ
            settextstyle(12, 0, _T("Consolas")); // �����ı�����ʹ�С
            
            // ���Ƴ���ʱ���ǩ
            outtextxy(labelX, labelY, durLabel);
        }
    }

    // ���ƽڵ��ǩ
    for (size_t i = 0; i < subproject.size(); ++i) {
        // �����ı���ɫ
        settextcolor(is_any_in_vector(criticalPath, subproject[i].id) ? RED : BLACK);
        // �����ı���ʽ
        TCHAR buffer[10];
        wsprintf(buffer, TEXT("%d"), subproject[i].id);
        // �����ı����뷽ʽ
        setbkmode(TRANSPARENT);
        // ���ƽڵ�ID��λ�ڽڵ�λ�õ�����
        outtextxy(positions[i].x - 5, positions[i].y - 7, buffer);
    }
}

// ���Ԫ���Ƿ���vector��
bool is_any_in_vector(const vector<int>& vec, int element) {
    return find(vec.begin(), vec.end(), element) != vec.end();
}

int main() {
    int n, m;
    cout << "�������ӹ���������>=10����";
    cin >> n;
    if (n < 10) {
        cout << "�ӹ����������벻����10��" << endl;
        return 1;
    }

    cout << "������������ϵ������>=20����";
    cin >> m;
    if (m < 20) {
        cout << "������ϵ�������벻����20��" << endl;
        return 1;
    }

    // �ӹ���
    vector<SubProject> subproject(n);

    // �ڽӱ�
    /*
    �ڽӱ���vector<vector<int>> graph�洢��
    graph[u]�洢���u�ĺ�̽�㣬��������ʽ���洢����id
    */
    vector<vector<int>> graph(n);

    // ����ÿ���ӹ��̵ĳ���ʱ��
    for (int i = 0; i < n; ++i) {
        subproject[i].id = i;
        cout << "�������ӹ��� " << i << " �ĳ���ʱ�䣺";
        cin >> subproject[i].duration;
        if (subproject[i].duration <= 0) {
            cout << "����ʱ�����Ϊ��������" << endl;
            --i;  // �������뵱ǰ�ӹ���
            continue;
        }
    }

    // ����������ϵ
    cout << "������������ϵ����ʽ��from to����" << endl;
    for (int i = 0; i < m; ++i) {
        int from, to;
        cin >> from >> to;
        if (from < 0 || from >= n || to < 0 || to >= n) {
            cout << "��Ч��������ϵ: " << from << " -> " << to << ". ���������롣" << endl;
            --i;  // �������뵱ǰ��
            continue;
        }
        if (from == to) {
            cout << "�Ի�������ϵ��Ч: " << from << " -> " << to << ". ���������롣" << endl;
            --i;  // �������뵱ǰ��
            continue;
        }
        // ����Ƿ��Ѿ�������ͬ��������ϵ
        if (find(graph[from].begin(), graph[from].end(), to) != graph[from].end()) {
            cout << "������ϵ " << from << " -> " << to << " �Ѵ��ڡ����������롣" << endl;
            --i;  // �������뵱ǰ��
            continue;
        }
        subproject[from].next.push_back(to);
        graph[from].push_back(to);
    }

    // �ж��Ƿ��л�
    if (ExistCycle(graph, n)) {
        cout << "��Ŀ����ѭ���������޷���ɡ�" << endl;
        return 1;
    }

    // ��������
    vector<int> topoOrder = topologicalSort(graph, n);
    cout << "�����������У�";
    for (int id : topoOrder) {
        cout << id << " ";
    }
    cout << endl;

    // ����ؼ�·��
    CriticalPathResult cpResult = findCriticalPath(subproject, topoOrder);
    cout << "�ؼ�·����";
    for (int id : cpResult.criticalPath) {
        cout << id << " ";
    }
    cout << endl;
    cout << "�ؼ�·�����ȣ�" << cpResult.criticalPathLength << endl;

    // ��ʼ�� EasyX ͼ�δ���
    initgraph(800, 600);  // ���ô��ڴ�С
    setbkcolor(WHITE);    // ���ñ�����ɫΪ��ɫ
    cleardevice();        // ����豸

    // ����ڵ�λ�ã����β��֣�ȷ�����нڵ���Բ���Ͼ��ȷֲ�
    /*
    ����ͼ�δ��ڵĿ�Ⱥ͸߶ȣ�800x600����ѡ���С��һ�ߣ�600����һ��Ϊ������
    ��ȥ100�������߾࣬����ڵ㿿�����ڱ�Ե
    */
    vector<NodePosition> positions(n);
    double angleStep = 2 * M_PI / n;
    double radius = min(800.0, 600.0) / 2 - 100;  // Բ�İ뾶
    double centerX = 400.0;  // Բ��X
    double centerY = 300.0;  // Բ��Y
    for (int i = 0; i < n; ++i) {
        double angle = i * angleStep - M_PI / 2;  // �Ӷ�����ʼ
        positions[i].x = static_cast<int>(centerX + radius * cos(angle));
        positions[i].y = static_cast<int>(centerY + radius * sin(angle));
    }

    // ����ͼ��
    drawGraph(subproject, topoOrder, cpResult.criticalPath, positions);

    // �ȴ��û��رմ���
    cout << "ͼ���ѻ��ơ�" << endl;
    system("pause");  // �ȴ��û�����
    closegraph();

    return 0;
}