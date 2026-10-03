#include<cstdio>
#include<algorithm>
#include<cmath>
using namespace std;
long long n,cnt=0;
long long x[5050],y[5050],s[5050];
struct edge
{
	long long u,v;
	double w;
}a[12500050];
void addedge(int u,int v)											//无向边的连接， 
{
	a[++cnt].u=u;
	a[cnt].v=v;
	a[cnt].w=sqrt((x[u]-x[v])*(x[u]-x[v])+(y[u]-y[v])*(y[u]-y[v]));
}
double comp(const edge &a,const edge &b)
{
	return a.w<b.w;
}
int search(int l)
{
	if(s[l]==l)
		return l;
	else 
		return s[l]=search(s[l]);
}
void linkl(int l,int m)
{
	s[search(l)]=search(m);
}
int main()
{
	int i,j,z=0;
	double result=0;
	scanf("%lld",&n);
	for(i=1;i<=n;i++)
		scanf("%lld%lld",&x[i],&y[i]);
	for(i=1;i<=n-1;i++)							//将题目没有给你建边的相关条件，假设全部节点都可以互相连接，且为无向边； 
		for(j=i+1;j<=n;j++)
			addedge(i,j);						//建边函数； 
	for(i=1;i<=n;i++)
		s[i]=i;									//并查集的初始化； 
	sort(a+1,a+1+cnt,comp);
	for(i=1;i<=cnt;i++)							//遍历所有已经建立好的无向边，；开始克鲁斯卡尔算法； 
	{
		if(search(a[i].u)!=search(a[i].v))
		{
			z++;
			linkl(a[i].u,a[i].v);
			result+=a[i].w;
		}
		if(z==n-1)
			break;
	}
	printf("%0.2lf",result);
	return 0;
}
