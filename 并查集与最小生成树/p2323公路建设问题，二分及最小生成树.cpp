#include <bits/stdc++.h>
using namespace std;
const int N=1e4+5,M=2e4+5;
int fa[N];
struct type {
	int u,v,w,tt;
	int ididi,choo;
}a[M],b[M];
vector<type> o;
int find(int x){
	if(x==fa[x]) return x;
	else return fa[x]=find(fa[x]);
}
void unite(int x,int y){
	fa[find(x)]=find(y);
}
bool cmp(type a,type b){
	return a.w<b.w;
}
bool cmp_(type a,type b){
	return a.tt<b.tt;
}
bool _cmp(type a,type b){
	return a.ididi<b.ididi;
}
int main()
{
	int n,k,m,ta,tb,tc1,tc2,m1=0,m2=0,ans=0;
	cin>>n>>k>>m;
	for(int i=1;i<=m-1;i++){
		cin>>ta>>tb>>tc1>>tc2;
		a[++m1].u=ta,a[m1].v=tb;
		a[m1].w=tc2,a[m1].tt=tc1,a[m1].ididi=i;
	}
	for(int i=1;i<=n;i++) fa[i]=i;
	sort(a+1,a+m1+1,cmp_);
	int y=n-1-k,i;
	for(i=1;i<=m1&&k;i++){
		if(find(a[i].u)!=find(a[i].v)){
			unite(a[i].u,a[i].v);
			ans=max(ans,a[i].tt);
			k--;
			a[i].choo=1;
			o.push_back(a[i]);
		}
	}
	sort(a+i,a+m1+1,cmp);
	for(;i<=m1;i++){
		if(find(a[i].u)!=find(a[i].v)){
			unite(a[i].u,a[i].v);
			ans=max(ans,a[i].w);
			a[i].choo=2;
			o.push_back(a[i]);
		}
	}
	cout<<ans<<endl;
	sort(o.begin(),o.end(),_cmp);
	for(int i=0;i<o.size();i++)
	    cout<<o[i].ididi<<' '<<o[i].choo<<endl;
	return 0;
}


////////////////////                                     ///////二分答案，枚举最大边权 
#include <bits/stdc++.h>
using namespace std;
int m1;
int n,k,m,ta,tb,tc1,tc2;
const int N=1e4+5,M=2e4+5;
int fa[N],vis[M],ans[M];
struct type {
	int u,v,w1,w2;
}a[M],b[M];
int find(int x){
	if(x==fa[x]) return x;
	else return fa[x]=find(fa[x]);
}
void unite(int x,int y){
	fa[find(x)]=find(y);
}
bool check(int pp){
    int cnt=0,ss=0;
	for(int i=1;i<=n;i++) fa[i]=i;
	memset(vis,0,sizeof(vis));
    for(int i=1;i<=m1;i++){
        if(find(a[i].u)!=find(a[i].v)){
			if(a[i].w1<=pp){
				vis[i]=1;
        	    cnt++;
        	    ss++;
        	    unite(a[i].u,a[i].v);
        	}
        }
    }
    for(int i=1;i<=m1;i++){
        if(find(a[i].u)!=find(a[i].v)){
        	if(a[i].w2<=pp){
        		vis[i]=2;
        		unite(a[i].u,a[i].v);
        		ss++;
			}
    	}
    }
    if(cnt>=k&&ss>=n-1){
        for(int i=1;i<=m1;i++) ans[i]=vis[i];
        return true;
    }
    else return false;
}
int main(){
	cin>>n>>k>>m;
	for(int i=1;i<=m-1;i++){
		cin>>ta>>tb>>tc1>>tc2;
		a[++m1].u=ta,a[m1].v=tb;
		a[m1].w1=tc1,a[m1].w2=tc2;
	}
	int l=0,r=30000,mid;
	while(l<r-1){
	    mid=(l+r)/2;
	    if(check(mid)) r=mid;
	    else l=mid;
	}
	cout<<r<<endl;
	for(int i=1;i<=m1;i++){
		if(ans[i]) cout<<i<<' '<<ans[i]<<endl;
	}
	return 0;
} 
