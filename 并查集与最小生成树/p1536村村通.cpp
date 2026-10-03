#include<iostream>
#include<cstring>
#include<algorithm>
using namespace std;
int set[1010];
int n,m,a,b;
int findx(int x){
	if(set[x]!=x)set[x]=findx(set[x]);
	return set[x];
}
void merge(int x,int y){
	int fx=findx(x),fy=findx(y);
	if(fx!=fy)set[fx]=fy;
}

int main(){
	while(1){
	    scanf("%d",&n);
		if(n==0)return 0;
		else scanf("%d",&m);
		for(int i=1;i<=n;i++){
			set[i]=i;
		}
		for(int i=1;i<=m;i++){
			scanf("%d %d",&a,&b);
			merge(a,b);
		}
		int cnt=-1;
		for(int i=1;i<=n;i++){
			if(set[i]==i)cnt++;
		}
		printf("%d\n",cnt);
		
	}
	
	
	
	
	return 0;
}
