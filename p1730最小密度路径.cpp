#include<bits/stdc++.h>
using namespace std;
int n,m;
typedef pair<int,int>pii;
vector<pii>g[55];
int q;
int ans=0;
using ll=long long;
double dist[55];
void dijkstra(int h,int start){
	memset(dist,0x3f,sizeof(dist));
	dist[start]=0.0;
	priority_queue<pii,vector<pii>,greater<pii>>que;
	que.push({0,start});
	while(!que.empty()){
		int u=que.top().second;
		int d=que.top().first;
		que.pop();
		if(d>dist[u])continue;
		for(int i=0;i<g[u].size();i++){
			if(g[u][i].second<dist[u]+g[u][i].second){
				g[u][i].second=dist[u]+g[u][i].second+0.0;
			}
		}
		
	}
}
int main(){
	scanf("%d",&n,&m);
	for(int i=1;i<=m;i++){
		int a,b,w;
		scanf("%d %d %d",&a,&b,&w);
		g[a].push_back({b,w});
	}
	scanf("%d",&q);
	while(q--){
		int x,y;
		scanf("%d %d",&x,&y);
		dijkstra(n,x);
		if(dist[y]==0x3f3f3f){
			printf("OMG!\n");
		}else{
		
		printf("%.3lf\n",dist[y]/m);
		}
	}
	
	
	
	return 0;
}
