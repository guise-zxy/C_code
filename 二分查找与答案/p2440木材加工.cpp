#include<bits/stdc++.h>
using namespace std;
int n,k;
int a[100010];
int m=0;
int check(int x){
	int sum=0;
	for(int i=1;i<=n;i++){
		sum+=a[i]/x;
	}
	if(sum<k)return 0;
	return 1;
}
int main(){
	scanf("%d %d",&n,&k);
	for(int i=1;i<=n;i++){
		scanf("%d",&a[i]);
		m=(m,a[i]);
	} 
	int l=0,r=m;
	while(l<r){
		int mid=(l+r+1)/2;
		if(check(mid)){
			l=mid;
		}else{
			r=mid-1;
		}
	}
	if( l>0){
		printf("%d",l);
	
	}else{
		cout<<0;
	}
	
	return 0;
}
