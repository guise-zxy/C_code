#include<bits/stdc++.h>
using namespace std;
int n,m,op,x,y;
int a[200010];
int findx(int g){
	int r=g;
	while(r!=a[g]){
		r=a[g];
	}
	return r; 
}
int merge(int c,int d){
	if(findx(c)!=findx(d)){
		a[findx(c)]=findx(d);
	}
} 
int main(){

		scanf("%d %d",&n,&m);
			for(int i=1;i<=n;i++){
				a[i]=i;
			}
		for(int i=1;i<=m;i++){
			scanf("%d %d %d",&op,&x,&y);
			if(op==1){
			
				merge(x,y);
			}
			else if(op==2){
			
				if(findx(x)!=findx(y)){
					printf("NO\n");
				}else{
					printf("YES\n");
				}
			}
			
		}	
	
	
	
	
	return 0;
}
