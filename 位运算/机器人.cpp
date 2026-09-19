#include<bits/stdc++.h>
using namespace std;
int m,n,res=0,d=1,an=0,bn=0;
vector<vector<bool>>a(50);
void dfs(int step){
	if(an==m&&bn==n){
		res++;
		return;
	}
	a[step].resize(step);
	vector<bool>&cur=a[step];
	for(int i=0;i<=1;i++){
		int an1=0,bn1=0;
		cur[0]=i;
		if(i)bn1++;
		else an1++;
		for(int j=1;j<step;j++){
			int num=cur[j-1]^
		} 
	}
	
	
	
	
}
int main(){
	
	
	
	
	return 0;
}
