#include<bits/stdc++.h>
using namespace std;
int a[10010];
int n,xb,i;
int num,ans;
bool check(int x){
	if(x+num>=a[n/2+1]){
		return true;
	}else{
		return false;
	}
}
int main(){
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		scanf("%d",&a[i]);
	}
	
	sort(a+1,a+n+1);
for(int i=1;i<=n;i++){
	int l=0,r=100010;
	num=a[i];
	if(a[i]>=a[n]){
		if(i!=1)printf(" ");
		printf("%d",a[i]);
		continue; 
	}
	if(i>n/2){
		printf(" 0");
	}
	while(l<r){
		int mid=(l+r)/2;
		if(check(mid)){
			r=mid;
		}else{
			l=mid+1;
		}
	}
	if(i!=1)printf(" ");
	printf("%d",l);
}
	
	return 0;
}
