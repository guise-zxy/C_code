#include<bits/stdc++.h>
using namespace std;
const int N=100010;
int top[N]={0};
struct node{
	int ts;
	int id;
}A[N];

bool cmp(node a,node b){
	if(a.id==b.id)return a.ts<b.ts;
	return a.id<b.id;
}
int n,d,k;


int main(){
	scanf("%d %d %d",&n,&d,&k);
	for(int i=1;i<=n;i++){
		scanf("%d %d",&A[i].ts,&A[i].id);
	}	
	sort(A+1,A+1+n,cmp);
	
	for(int i=1,j=k;j<=n;i++,j++){
		int ans=0;
		for(int x=i;x<=j;x++){
			if(A[x].id!=A[j].id){
				break;
			}else if(A[x].ts+d-1>=A[j].ts){
				ans++;
			}
			if(ans>=k)top[A[i].id]=1;
		}
	}
	
	for(int q=0;q<=N;q++){
		if(top[q]){
			printf("%d\n",q);
		}
	} 
	
	
	
	return 0;
}
























//#include<bits/stdc++.h>
using namespace std;
int n,d,k,cnt=0;
struct ii{
	int x,y;
}a[100010];
struct pp{
	int d,f;
};
pp b[100010];
bool cmp(ii xx,ii yy){
	return xx.x<=yy.x&&xx.y<yy.y;

}
int main(){
	scanf("%d %d %d",&n,&d,&k);
	for(int i=1;i<=n;i++){
		scanf("%d %d",&a[i].x,&a[i].y);
		(b[a[i].y].d)++;
		b[a[i].y].f=min(b[a[i].y].f,a[i].x);
	} 
	sort(a+1,a+1+n,cmp);
	for(int i=1;i<=n;i++){
		if(b[i].d<k){
			continue;
		}
		else if(b[i].d>=k){
			if(b[i].f<=d){
				printf("%d\n",i);
			}
		}
	}
	return 0;
}
