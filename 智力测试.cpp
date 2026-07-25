#include<bits/stdc++.h>
using namespace std;
int n,m,t;
int C[100010],R[100010];
int s1,s2,t1,t2;
struct edge{
	int x1;
	int x2;
};
vector<edge>g(10000000000);
int ans=0;
int dx[]={0,1,0,-1};
int dy[]={1,0,-1,0};
bool cmp(edge a1,edge a2){
	return (a1.x1<=a2.x1)&&(a1.x2<a2.x2);
}


void dfs(int x1,int x2,int y1,int y2){
	if(x1>n||y1>m||x2>n||y2>m){
		return ;
	}
	if(x1==x2&&y1==y2){
		ans++;
		return;
	}
	
	for(int i=0;i<4;i++){
		int p=x1+dx[i],o=y1+dy[i];
		if(p>n||o>m)continue;
		dfs(p,o,y1,y2);
	}
	
}




int main(){
	scanf("%d %d %d",&n,&m,&t);
	for(int i=1;i<=n;i++){
		scanf("%d",&R[i]);
		g[i].x1=R[i];
	}	
	
		for(int i=1;i<=n;i++){
		scanf("%d",&C[i]);
		g[i].x2=C[i];
	}	
	sort(g+1,g+1+n,cmp);
	while(t--){
		scanf("%d %d %d %d",&s1,&s2,&t1,&t2);
		dfs(s1,s2,t1,t2);
		printf("%d",ans);
		ans=0;
	}
	return 0;
}
