#include<bits/stdc++.h>
using namespace std;
int l,n,k;
int j[100010];
int s[100010];
int check(int x){
	int cnt=0;
	for(int i=1;i<=n;i++){
		if(s[i]>x){
			cnt++;
		int num=s[i]-x;
		while(num>x){
			cnt++;
			num-=x;
		}
		}
	  
		
	}
	if(cnt>k)return 0;
	return 1;

}
int main(){
	scanf("%d %d %d",&l,&n,&k);
	int highest=0;
	for(int i=1;i<=n;i++){
		scanf("%d",&j[i]);
		s[i]=j[i]-j[i-1];
		highest=max(highest,j[i]);
	}
	
	int le=0,r=highest;
	while(le<r){
		int mid=(le+r)>>1;
		if(check(mid)){
			r=mid;
		}else{
			le=mid+1;
		}
		
	}
	printf("%d",le);
	
	
	return 0;
}
