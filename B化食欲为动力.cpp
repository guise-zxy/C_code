#include<bits/stdc++.h>
using namespace std;
const int N=210;
long long a[N],b[N],c[N];
int n,m,k;
long long ans=0;
int main(){
		scanf("%d %d %d",&n,&m,&k);
		for(int i=1;i<=n;i++){
			scanf("%d",&a[i]);
		}
		
		for(int i=1;i<=m;i++){
			scanf("%d",&b[i]);
		}
		
		for(int i=1;i<=k;i++){
				scanf("%d",&c[i]);
			}
		
		sort(a+1,a+1+n);
		sort(b+1,b+1+m);
		sort(c+1,c+1+k);
		ans=(long long)(a[n]*b[m])%c[1];
		printf("%lld",ans);
	return 0;
}
