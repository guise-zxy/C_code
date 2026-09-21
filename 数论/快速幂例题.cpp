#include<bits/stdc++.h>
using namespace std;
long long b,p,k;
long long ans=1;

int main(){
	scanf("%lld %lld %lld",&b,&p,&k);
	while(p){
		if(p&1)ans=(ans*b)%k;
		p>>=1;
		b=(long long)b*b%k; 
	}
	
	printf("%lld\n",ans);
	
	
	return 0;
}
