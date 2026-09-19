#include<bits/stdc++.h>
using namespace std;
int n,m;
int dist[1000010];
int visit[1000010];
#define pii pair<int,int>
#define p 100003
vector<pii>g[4000010];
int t=0;
int nextt;
int main(){
	scanf("%d %d",&n,&m);
	memset(dist,0x3f,sizeof(dist));
	while(m--){
	
		int x,y;
		if(x==y){
			continue;
		}
		scanf("%d %d",&x,&y);
		g[x].push_back({y,1});
		g[y].push_back({x,1});
	}
	
	for(int k=1;k<=n;k++){
		memset(dist,0x3f,sizeof(dist));
		memset(visit,0,sizeof(visit));
		dist[1]=0;
		int now=1;
		int index=1,MIN=0x3f3f3f;
		while(now!=k){
			for(int i=1;i<=n;i++){
				if(!visit[i]&&g[now][i].second){
					dist[i]=min(dist[i],dist[now]+g[now][i].second);
				}
				if(dist[i]<MIN){
					MIN=dist[i];
					nextt=i;
					
				}
				
				
			}
			now=nextt;
			visit[now]=1;
		}
			printf("%d\n",dist[k]%p);
	}

	
	return 0;	
}
