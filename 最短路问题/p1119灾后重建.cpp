#include<bits/stdc++.h>
using namespace std;

const int MAX_N = 200;
const int INF = 0x3f3f3f3f;

vector<pair<int, int>> adj[MAX_N]; // 邻接表：adj[u]存储{v, w}
int village_time[MAX_N];          // 村庄重建时间
int dist[MAX_N];                  // 最短距离数组

void dijkstra(int start, int current_t) {
    memset(dist, 0x3f, sizeof(dist));
    dist[start] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto current = pq.top();
        int u = current.second;
        int current_dist = current.first;
        pq.pop();

        // 跳过无效记录或非法节点
        if (current_dist > dist[u] || village_time[u] > current_t) continue;

        for (auto& edge : adj[u]) {
            int v = edge.first;
            int weight = edge.second;

            // 检查邻接节点是否合法
            if (village_time[v] > current_t) continue;

            // 松弛操作
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
}

int main() {
    int n, m, q;
    scanf("%d%d%d", &n, &m, &q);

    // 输入村庄重建时间（村庄编号从0到n-1）
    for (int i = 0; i < n; i++) {
        scanf("%d", &village_time[i]);
    }

    // 输入边（无向图）
    for (int i = 0; i < m; i++) {
        int u, v, w;
        scanf("%d%d%d", &u, &v, &w);
        adj[u].emplace_back(v, w);
        adj[v].emplace_back(u, w);
    }

    // 处理查询
    while (q--) {
        int x, y, current_t;
        scanf("%d%d%d", &x, &y, &current_t);

        // 特殊情况处理：起点和终点相同
        if (x == y) {
            printf("%d", 0);
            continue;
        }

        // 检查起点和终点是否合法
        if (village_time[x] > current_t || village_time[y] > current_t) {
            printf("-1");
            continue;
        }

        // 运行Dijkstra算法
        dijkstra(x, current_t);

        // 输出结果
        if (dist[y] == INF) {
            printf("-1");
        } else {
            printf("%d", dist[y]);
        }
    }

    return 0;
}
