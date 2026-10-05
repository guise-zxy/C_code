#include<bits/stdc++.h>
using namespace std;
int n,m;
int visit[10101];
int sl[10101];
int f[10101];
void print(){
	for(int i=1;i<=n;i++){
		printf("%d",sl[i]);
	}
	printf("\n");
} 
void dfs(int cur){
	if(cur==n+1){
		m--;
		
	}
	if(m==-1){
		print();
		return;
	}
	else for(int i=(f[cur]?1:sl[cur]);i<=n;i++){
		if(!visit[i]){
			sl[cur]=i;
			visit[i]=1;
			dfs(cur+1);
			visit[i]=0;
		}
	
	  }
	
		f[cur]=1;
	
}

int main(){
	
	    scanf("%d%d",&n,&m);
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&sl[i]);
        //vis[i]=1;
    }
    dfs(1);
	
	
	
	
	return 0;
}


