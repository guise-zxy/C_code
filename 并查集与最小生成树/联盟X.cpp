#include<bits/stdc++.h>
using namespace std;
int n,m;
int a[20010];
int siz[20010];
int x,y; 
int findx(int g){
	if(a[g]!=g){
		a[g]=findx(a[g]);
	}
	return a[g];
}
int merge(int c,int d){
	if(findx(c)!=findx(d)){
		a[findx(c)]=findx(d);
		siz[findx(d)]+=siz[findx(c)];
	}
} 
int main(){
	scanf("%d %d",&n,&m);
	for(int i=1;i<=n;i++){
		a[i]=i;
		siz[i] = 1;  // 初始化每个联盟的大小为1
	}

	for(int i=1;i<=m;i++){
		scanf("%d %d",&x,&y);
		merge(x,y);
	}
	int ans=0;
	for(int i=1;i<=n;i++){
	ans=min(ans,siz[i]);
	}
	printf("%d",ans);
	return 0;
}
