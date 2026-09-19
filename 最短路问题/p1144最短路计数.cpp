#include<iostream>
#include<cstring>
#include<algorithm>
using namespace std;
#define inf 0x7FFFFFFF
int n,m,visit[1000];
int g[1000][1000];
int x,y,cnt=0,next;
int dist[100000];


int main(){
	
	scanf("%d %d" ,&n,&m);
	for(int i=1;i<=n;i++){
		dist[i]=inf;
		visit[i]=1;
		
	}
	for(int i=1;i<=m;i++){
		scanf("%d %d" ,&x,&y);
		if(x==y){
			g[x][y]+=1;
		}else{	
			g[x][y]++;
			g[y][x]++;
		}
	
	}
	int Min;
	int now=1;
	dist[now]=1;
	visit[now]=1;
	for(int k=1;k<=n;k++){
		now=1;
		while(now!=k){
			Min =inf;
		   for(int i=1;i<=n;i++){
		   
			if(g[now][i]!=0){
			
				dist[i]=min(dist[i],g[now][i]+dist[now]);
			}
			if(visit[i]&&dist[i]<Min){
				next=i;
				Min =dist[i];
			}
			
		
		}
		if(Min==inf)break;
		now=next;
		visit[now]=0;
		
	}
printf("%d\n",dist[k]);


}

	
	
	return 0;
}
