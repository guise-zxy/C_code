#include<bits/stdc++.h>
using namespace std;
long long n,m;
int t[1000010];
long long ans=0,sum=0;
 int check(int x){
 	sum=0;
 	for(int i=1;i<=n;i++){
 		sum+=max(0,t[i]-x);
 	
	 }
	 if(sum<m){
	 	return 0;
	 }else{
	 	return 1;
	 }
 }
int mm=0;
int main(){
	scanf("%lld %lld",&n,&m);
	for(int i=1;i<=n;i++){
		scanf("%d",&t[i]);
		mm=max(mm,t[i]);
	}
	long long l=0,r=mm;
	while(l<r){
		int mid=l+r+1>>1;
		if(check(mid)){
		if(ans<mid)ans=mid; 
			l=mid;
		}else{
			r=mid-1;
		}
	}
	printf("%d",ans);
	
	
	
	return 0;
}
