#include<iostream>
#include<cstring>
#include<algorithm>
using namespace std;
int n,m,p;
int a,b;
int p1,p2,set[5010];
int findx(int x){
	if(set[x]!=x)set[x]=findx(set[x]);
	return set[x];
}
void merge(int x,int y){
	int fx=findx(x),fy=findx(y);
	if(fx!=fy)set[fx]=fy;
}




int main(){
	scanf("%d %d %d",&n,&m,&p);
	for(int i=1;i<=n;i++){
		set[i]=i;
	}
	for(int i=1;i<=m;i++){
		scanf("%d %d",&a,&b);
		merge(a,b);
	}
	
	for(int i=1;i<=p;i++){
		scanf("%d %d",&p1,&p2);
		if(findx(p1)==findx(p2))printf("Yes\n");
		else printf("No\n");
	}
	
	
	
	
	
	return 0;
}
