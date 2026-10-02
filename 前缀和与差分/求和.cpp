#include<bits/stdc++.h>
using namespace std;
#define ll long long 
ll n;
ll s=0;
ll sum[200010];
ll a[200010];
 
int main(){
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		scanf("%lld",&a[i]);
		sum[i]=a[i]+sum[i-1];
	}
	
	for(int i=1;i<=n;i++){
		s+=a[i]*(sum[n]-sum[i]);
	}
	
	printf("%lld",s);
	
	
	
	
	return 0;
} 
