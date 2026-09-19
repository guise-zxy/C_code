#include <bits/stdc++.h>
using namespace std;
using ll = long long;
typedef pair<int, int> pii;
const int MAX = 2147483647;
const int N = 10000;

vector<vector<pii>> graph(N + 1); // 邻接表存储图
int dist[N + 1]; // 距离数组

void dijkstra(int n, int start) {
    // 初始化距离数组
    for (int i = 1; i <= n; i++) {
        dist[i] = MAX;
    }
    dist[start] = 0;

    // 优先队列，存储 <距离, 节点>
    priority_queue<pii, vector<pii>, greater<pii>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second; // 当前节点
        int d = pq.top().first;  // 当前距离
        pq.pop();

        // 如果当前距离大于已知的最短距离，跳过
        if (d > dist[u]) continue;

        // 遍历邻接节点
        for (int i=1; i<graph[u].size();i++) {
            int v =graph[u][i] .first;    // 邻接节点
            int weight =graph[u][i].second; // 边权

            // 如果找到更短的路径
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
}

int main() {
    int n, m, s;
    scanf("%d %d %d", &n, &m, &s); // s 为起点

    // 邻接表存图
    for (int i = 1; i <= m; i++) {
        int a, b, c;
        scanf("%d %d %d", &a, &b, &c);
        graph[a].push_back({b, c}); // 有向图
    }

    dijkstra(n, s);

    // 输出结果
    for (int i = 1; i <= n; i++) {
        if (dist[i] == MAX) {
            printf("2147483647 ");
        } else {
            printf("%d ", dist[i]);
        }
    }
    printf("\n");

    return 0;
}
