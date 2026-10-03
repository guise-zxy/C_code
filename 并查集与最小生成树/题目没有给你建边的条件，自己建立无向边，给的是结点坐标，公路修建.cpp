#include<bits/stdc++.h>
using namespace std;
typedef pair<int,int>pii;
int f[5010];
struct dl{
	int x;int y;
	double c;
}g[5010];
int findx(int x){
	while(f[x]!=x){
		f[x]=f[f[x]];
		x=f[x];
	}
	return x;
}
double len;
void merge(int x,int y ){
	int fx=findx(x),fy=findx(y);
	if(fx!=fy){
		f[fx]=fy;
	}
}


bool cmp(dl x,dl y){
	return x.c<y.c;
}

int n,k,l,cnt=0;

double ans;

int main(){
	
	for(int i=1;i<=n;i++){
		scanf("%d %d",&k,&l);
		len=sqrt(k*k+l*l);
		g[i].x=k;
		g[i].y=l;
		g[i].c=len;
	cnt++;
		
	}
	
	for(int i=1;i<=n;i++){
		f[i]=i;
	}
	int ccc=0;
	sort(g+1,g+1+n,cmp);
	for(int i=1;i<=cnt;i++){
		if(findx(g[i].x)!=findx(g[i].y)){
			merge(g[i].x,g[i].y);
			ans+=g[i].c;
			ccc++;
		}
		
		if(ccc==cnt-1){
			break;
		}
	}
	
	printf("%.2f",ans);
	
	
	
	
	return 0;
}
