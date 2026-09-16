#include<bits/stdc++.h>
using namespace std;
int n,x[200005],y[200005],s[200005];
long double opt=1e-8,ans=0;



int check(double x){
}
int main(){
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		scanf("%d %d %d",&x[i],&y[i],&s[i]);
	}	
	
	int l=0,r=1e7;
	while(r-l>opt){
		long double mid=(long double)(l+r)/2,t=0;
		bool ck=1;
		for(int i=1;i<=n;i++){
			t+=s[i]/mid;	
			if(t<x[i]){
				t=x[i];
			}
			if(t>y[i]){
				ck=0;
				break;
			}
		
		}
		if(ck){
			r=mid-opt;
		}else{
			ans=max(ans,mid);
			l=mid+opt;
		}
	}
	
	
	printf("%.2LF",ans);
	
	return 0;
}
