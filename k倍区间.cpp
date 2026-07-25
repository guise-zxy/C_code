#include<bits/stdc++.h>
using namespace std;
int n,k;
int a[100010];
int sum[100010];
int ans=0;
int main(){
	scanf("%d %d",&n,&k);
	for(int i=1;i<=n;i++){
		scanf("%d",&a[i]);
		sum[i]=sum[i-1]+a[i];
	}
	for(int i=1;i<=n;i++){
		for(int j=i;j<=n;j++){
			if((sum[j]-sum[i-1])%k==0)ans++;
		}
	}
	
	printf("%d\n",ans);
	
	return 0;
}
