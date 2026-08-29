#include<bits/stdc++.h>
using namespace std;
int m;
int sum=0;
int a[2000010];
int main(){
	for(int i=1;i<=m;i++){
		a[i]=i;
	}
	for(int i=1;i<=m;i++){
		for(int j=n;j>1;j--){
			while(j>l){
				sum=(a[i]+a[j])*(j-i+1)/2;
				if(sum>m)j--;
				else if(sum<m){
					i--
				}
			}
		}
	}
	return 0;
}
