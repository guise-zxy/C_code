#include<bits/stdc++.h>
using namespace std;
int ksm(int a,int b,int p){
	int res;  
	while(b){
		if(b&1) res=(long long )res*a%p;
		b>>=1;
		a=(long long)a*a%p;
	}
	
	
	return res;
}


int main(){
		int n,a,b,p;
		cin>>n;
		while(n--){
			cin>>a>>b>>p; 
			cout<<ksm(a,b,p);
		} 
	
	return 0;
}
