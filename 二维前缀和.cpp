#include<bits/stdc++.h>
using namespace std;
int n,m,q;
const int N=1010;
int a[N][N];
int sum[N][N];
int main(){
	scanf("%d %d %d",&n,&m,&q);
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			scanf("%d",&a[i][j]);
			
		}
	}
	
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
		
			if(i==1)sum[1][j]=a[1][j]+sum[1][j-1];
			if(j==1)sum[i][1]=a[i][1]+sum[i-1][1];
			if(i!=1&&j!=1){
				sum[i][j]=a[i][j]+sum[i-1][j]+sum[i][j-1]-sum[i-1][j-1];
			}
		}
	}
	
	while(q--){
		int x1,y1,x2,y2;
		scanf("%d %d %d %d",&x1,&y1,&x2,&y2);
		printf("%d\n",sum[x2][y2]-sum[x2][y1-1]-sum[x1-1][y2]+sum[x1-1][y1-1]);
	}
	
	
	
	return 0;
}
