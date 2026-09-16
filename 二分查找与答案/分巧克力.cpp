#include<bits/stdc++.h>
using namespace std;
const int N=100010;
int n,k;
typedef pair<int ,int>pii;
vector<pii>a(N);
bool check(int x){
	long long ans=0;
	for(int i=1;i<=n;i++){
		ans+=(a[i].first/x)*(a[i].second/x);
	}
	if(ans>=k)return 1;
	else{
		return 0;
	}
	
	
	
}


int main(){
	scanf("%d %d",&n,&k);
	for(int i=1;i<=n;i++){
		scanf("%d %d",&a[i].first,&a[i].second);
	}
	
	int l=1,r=0;
	for(int i=1;i<=n;i++){
		r=max(r,min(a[i].first,a[i].second));
	}
	while(l<r){
		int mid=(l+r+1)/2;
		if(check(mid)){
			l=mid;
		}else{
			r=mid-1;
		}
	}
	
	printf("%d",l);
	return 0;
}

