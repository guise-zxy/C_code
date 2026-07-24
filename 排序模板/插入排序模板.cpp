#include<bits/stdc++.h>
using namespace std;
const int N=10010;
int a[N];
int main(){
	for(int i=2;i<=N;i++){
		int key=a[i];
		for(int j=i-1;j<=N;j++){
			if(j>=1&&a[j]>key){
				a[j+1]=a[j];
				j--;
			}
			a[j+1]=key;		
		}
		
	}
	
	return 0;
}
