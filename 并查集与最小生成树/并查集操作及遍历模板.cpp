#include<iostream>
#include<cstring>
#include<algorithm>
using namespace std;
const int N=1010;
int set[N];

int findx(int x){				/////函数名find()可能与库里头的有冲突；所以加x; 
	int r=x;
	while(set[r]!=r){		//查找并查集的老大； 
		r=set[x]; 
	}
	return r; 
	
}

void merge(int x,int y){
		int fx,fy;			//进行合并并查集的操作，此操作的对象必为两个两个并查集的老头，即头； 
		fx=findx(x);         //经过此函数，两个节点已经是一种树的形态了； 不行就画图自己看； 
		fy=findx(y);
		if(fx!=fy){
			set[fx]=fy;
	
	}

}





int main(){
	
	
	
	
	
	return 0;
}
