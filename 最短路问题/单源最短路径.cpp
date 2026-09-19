#include<bits/stdc++.h>
using namespace std;
int n,m,s;
struct edge{												////////此为用邻接表的数据结构来存！！！!!! 
    int v;
	int w; // v表示边的终点，w表示边的权重
};
#define INF 1e9
vector<edge> e[500010]; // 邻接表存储图，e[u]表示从节点u出发的所有边
int dis[500010]; // dis[u]表示从起点到节点u的最短距离
int vis[500010]; // vis[u]表示节点u是否已经被访问（是否已经确定最短路径）

void dijkstra(int n, int s) {
    // 初始化所有节点的距离为无穷大
    memset(dis, 0x3f, sizeof(dis));
    dis[s] = 0; // 起点到自身的距离为0

    // 遍历所有节点，依次确定每个节点的最短路径
    for (int i = 1; i <= n; i++) {
        int u = 0, mind = 0x3f3f3f3f; // u表示当前要确定最短路径的节点(即为起点)，mind用于记录最小距离
        // 寻找未访问的节点中距离起点最近的节点
        for (int j = 1; j <= n; j++) {																				
            if (!vis[j] && dis[j] < mind) {
                u = j; // 更新当前节点
                mind = dis[j]; // 更新最小距离
            }
        }
        vis[u] = 1; // 标记当前节点为已访问

        // 遍历当前节点u的所有邻边，更新其邻接点的最短距离
        for (int i=0;i<=e[u].size();i++) {
            int h = e[u][i].v, w= e[u][i].w; // v为邻边的终点，w为边的权重
            // 如果通过当前节点u到达v的距离更短，则更新dis[v]
            if (dis[h] > dis[u] + w) {						///////////min()
                dis[h] = dis[u] + w;
            }
        }
    }
}


int main(){
	scanf("%d %d %d",&n,&m,&s);
	for(int i=1;i<=m;i++){
		int u,v,w;
		scanf("%d %d %d",&u,&v,&w);
		e[u].push_back({v,w});
	
	}
	 
	 dijkstra(n,s);
	 for (int i = 1; i <= n; i++) {
        // 处理无法到达的情况
        if (dis[i] == INF) {
            printf("%d",(1 << 31) - 1);
        } else {
            printf("%d", dis[i]);}
	return 0;
}
}
