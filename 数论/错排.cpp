#include<bits/stdc++.h>
using namespace std;
int d[30]={0,1};

int cp(int x){


	
	if(x==0)return d[0];
	if(x==1)return d[1];
	else{
		return d[x]=(x-1)*(d[x-1]+d[x-2]); 
	} 
} 
int main(){d[0]=0;
	d[1]=1;
	int n;
	while(scanf("%d",&n)==1){
		printf("%d\n",cp(n));
	}
	
	return 0; 
}
