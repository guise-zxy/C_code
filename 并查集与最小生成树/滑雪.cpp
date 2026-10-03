#include<iostream>
#include<cstring>
#include<cstdio>
#include<cmath>
#include<algorithm>
#define ll long long
#define INF 0x7fffffff
#define re register

using namespace std;

int read()
{
    register int x = 0,f = 1;register char ch;
    ch = getchar();
    while(ch > '9' || ch < '0'){if(ch == '-') f = -f;ch = getchar();}
    while(ch <= '9' && ch >= '0'){x = x * 10 + ch - 48;ch = getchar();}
    return x * f;
}

struct edge1{
	int next,to,k,from;
}edge[2000005];

struct edge2{
	int x,y,k;
}e[2000005];

long long n,m,u,v,k,t,cnt,ans,tot,h[100005],d[100005],vis[100005],fa[100005];

void add(int x, int y, int k)
{
	edge[++cnt].to = y;
	edge[cnt].k = k;
	edge[cnt].next = d[x];
	edge[cnt].from = x;
	d[x] = cnt;
}

void dfs(int s)
{
	vis[s] = 1;
	for(re int i = d[s]; i; i = edge[i].next) if(!vis[edge[i].to]) dfs(edge[i].to);
}

int mysort(edge2 a, edge2 b)
{
	if(h[a.y] != h[b.y]) return h[a.y] > h[b.y];
	return a.k < b.k;
}

int find(int x) {if(fa[x] == x) return x; return fa[x] = find(fa[x]);}

void kruskal()
{
	for(re int i = 1; i <= n; i++) fa[i] = i;
	sort(e + 1, e + t + 1, mysort);
	for(re int i = 1; i <= t; i++)
	{
		if(find(e[i].x) != find(e[i].y))
		{
			ans = ans + e[i].k;
			fa[find(e[i].x)] = find(e[i].y);
		}
	}
}

int main()
{
	n = read(); m = read();
	for(re int i = 1; i <= n; i++) h[i] = read();
	for(re int i = 1; i <= m; i++)
	{
		u = read();  v = read(); k = read();
		if(h[u] >= h[v]) add(u,v,k);
		if(h[u] <= h[v]) add(v,u,k);
	}
	dfs(1);
	for(re int i = 1; i <= cnt; i ++)
	{
		if((edge[i].from != edge[i - 1].to || edge[i].to != edge[i - 1].from) && vis[edge[i].from] && vis[edge[i].to])
		{
			e[++t].k = edge[i].k;
			e[t].x = edge[i].from;
			e[t].y = edge[i].to;
		}
	}
	kruskal();
	for(re int i = 1; i <= n; i++) if(vis[i]) tot ++;
	printf("%lld %lld\n", tot, ans);
    return 0;
}
