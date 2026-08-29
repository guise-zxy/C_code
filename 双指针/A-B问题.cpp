#include<bits/stdc++.h>
using namespace std;
int n,c;
int a[200010];
int main(){
	scanf("%d %d",&n,&c);
	for(int i=1;i<=n;i++){
		scanf("%d",&a[i]);
	}
	int ans=0;
	for(int i=1;i<=n;i++){
		for(int j=i+1;j<=n;j++){
			if(a[j]-a[i]==c)ans++;
		}
	}
	printf("%d",ans);
	
	return 0;
}
