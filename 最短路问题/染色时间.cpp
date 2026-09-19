#include<bits/stdc++.h>
using namespace std;
int n,m;
typedef pair<int,int>PII;
int dx[5]={0,0,1,0,-1};
int dy[5]={0,1,0,-1,0};
int t[510][510];
int st[510][510];
PII q[510*510];
int cnt=0;
void bfs(int x1,int y1){
	q[0]={x1,y1};
	st[x1][y1]=1;
	int hh=0,tt=0;
	while(hh<=tt){
		auto mm=q[hh++];
		printf("%d %d",tt,hh);
		for(int i=1;i<=5;i++){
			int a=mm.first+dx[i],b=mm.second+dy[i];
			if(a<1||a>=n||b<1||b>=n){
				continue;
			}
			if(st[a][b])continue;
			st[a][b]=1;
			t[a][b]=min(t[a][b],t[x1][y1]);
			cnt+=1;
			q[++tt]={a,b};	
		}
		return ;
	}
	
	
	
	
	
}
int main(){
	scanf("%d %d",&n,&m);
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			scanf("%d",&t[i][j]);
		
		}
	}
	bfs(1,1);
	
	printf("%d"cnt);
	
	
	return 0;
}
