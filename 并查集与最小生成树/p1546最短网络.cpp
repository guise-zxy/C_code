#include<bits/stdc++.h>
using namespace std;
int n;
struct bc{
	int x,y,c;
}g[10010];
int f[10010];
bool cmp(bc x1,bc x2){
	return x1.c<x2.c;
}

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
int t=0,k;
int main(){
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		f[i]=i;
	
			for(int j=1;j<=n;j++){		
			scanf("%d",&k);				//一定要读入； 
				if(j>i){					//j>i,判断是不是，j==i,不存在自己连自己边； 
					t++;
					g[t].x=i;g[t].y=j,g[t].c=k;
				}
			}
}
	int cnt=0,mt=0;
	sort(g+1,g+1+t,cmp);
	for(int i=1;i<=t;i++){
		if(findx(g[i].x)!=findx(g[i].y)){
			mt+=g[i].c;
			f[findx(g[i].x)]=findx(g[i].y);
			cnt++;
		}
		if(cnt==n-1){
			break;
		}
		
	}
	
	printf("%d",mt);
	
	return 0;
}
