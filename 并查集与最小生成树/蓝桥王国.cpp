#include <bits/stdc++.h>
using namespace std;
const int N=3e5+10;
#define ll long long
ll n,m,w,u,v;
ll dist[N];
ll g[N][N];
bool visit[N];
int dijkstra(int x){
  memset(dist,0x3f3f3f,sizeof(dist));
  dist[1]=0;
  int t=-1;
  for(int i=1;i<=n;i++){
    for(int j=1;j<=n;j++){
      if(!dist[j]&&(t==-1||dist[t]>dist[j])){
        t=j;
      }
    }
    if(t==-1||dist[t]==0x3f){
         break;
    }
    visit[t]=1;
    for(int j=1;j<=n;j++){
      if(dist[j]>dist[t]+g[t][j]){
        dist[j]=dist[t]+g[t][j];
      }
    }

  }

  if(dist[x]==0x3f3f3f){
    return -1;
  }
  return dist[x];


}
int main()
{
  memset(g,0x3f,sizeof(g));
  
  scanf("%d %d",&n,&m);
  for(int i=1;i<=m;i++){
    scanf("%d %d %d",&u,&v,&w);
    g[u][v]=min(g[u][v],w);
  }
  dijkstra(1);
  for(int i=1;i<=n;i++){
  
    if(i!=1)printf(" ");
    printf("%d",dist[i]);
  }
  
  return 0;
}
