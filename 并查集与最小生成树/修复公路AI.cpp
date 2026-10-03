#include<iostream>
#include<cstring>
#include<algorithm>
using namespace std;
int n,m,xx;
struct edge{
	int x,y,t;
}g[10100];
int f[10100];

bool cmp(edge x1,edge x2){
	return x1.t<x2.t;
}

// 改为非递归实现以避免栈溢出
int findx(int x) {
    while (f[x] != x) {
        f[x] = f[f[x]]; // 路径压缩，循环实现 
        x = f[x];
    }
    return x;
}
void merge(int x,int y){
	int fx=findx(x),fy=findx(y);
	if(fx!=fy)f[fx]=fy;
}




/*int findx(int x){   	//路径压缩，递归实现 ，//可能会栈溢出！！！！！ 
	if(f[x]!=x)f[x]=findx(f[x]);
	return f[x];
}
void merge(int x,int y){
	int fx=findx(x),fy=findx(y);
	if(fx!=fy)f[fx]=fy;
}*/


int main(){
	scanf("%d %d",&n,&m);

	
	for(int i=1;i<=m;i++){
		scanf("%d %d %d",&g[i].x,&g[i].y,&g[i].t);
	}
 	sort(g+1,g+m+1,cmp);
 	
	for(int i=1;i<=n;i++){
		f[i]=i;
	}
	int ccc=0,mt=0;
	for(int i=1;i<=m;i++){
		if(findx(g[i].x)==findx(g[i].y))continue;
		else {
		  	merge(g[i].x, g[i].y);
			ccc++;
			mt+=g[i].t;		
		}
		if(ccc==n-1)break;
	}
	int cnt = 0;
    for (int i = 1; i <= n; ++i) {
        if (findx(i) == i) cnt++;
    }
    
	
	if(ccc!=n-1|| cnt > 1)printf("orz");
	else printf("%d",mt);

	
	
	
	return 0;}
