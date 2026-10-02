#include<bits/stdc++.h>
using namespace std;
int n,m,c=1;
int a[110][110];
int sum[110][110];
int ans=0;
int main(){
	scanf("%d %d",&n,&m); 
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			scanf(" %d",&a[i][j]);
			sum[i][j]=a[i][j]+sum[i][j-1]+sum[i-1][j]-sum[i-1][j-1];
		}
	} 
	
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			if (a[i][j] == 0) continue; // 剪枝：起点必须为1
			  c=min(n-i,m-j);  
			  
		for(int k=0;k<=c;k++){                                                                                                                                                                  	
			if (i+k > n || j+k > m) break; // 边界检查
				if((sum[i + k][j + k] - sum[i + k][j - 1] - sum[i - 1][j + k] + sum[i - 1][j - 1])==(k+1)*(k+1))ans=max(k+1,ans);
		
		
			}
		
		}
	}
	printf("%d",ans);
	
	return 0;
} 
