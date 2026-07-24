#include<bits/stdc++.h>
using namespace std;
int n=100;
int a[n];	//最初序列并更新已排序好的的序列
int c[n];		//记录数值个数的额外数组； 
int main(){
	for(int i=1;i<=n;i++){
		c[a[i]]++;
	} 
	int dd=0;   //新的下标 
	for(int i=ymin;i<=ymax;y++){
		while(c[i]>0){
			a[++dd]=i;
			c[i]--;
		} 
	}
	
	
	
	return 0;
} 
