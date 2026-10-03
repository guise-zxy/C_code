#include<cstdio>
#include<cstring>
#include<cmath>
#define eps 1e-4
int c[10100],n,m;
int x[1010],y[1010];
int my_find(int x)
{
    if(c[x]==x)
        return x;
    return c[x]=my_find(c[x]);
}
bool check(double ans)
{
    for(int i=1;i<=n;i++)
        c[i]=i;
    int cnt=0;
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++)
            if((x[j]-x[i])*(x[j]-x[i])+(y[j]-y[i])*(y[j]-y[i])<=ans)//暂时不开根号
                c[my_find(i)]=my_find(j);
    for(int i=1;i<=n;i++)
        if(my_find(i)==i)
            cnt++;
    if(cnt<m)//框多了
        return false;
    return true;
}
int main()
{
    double mx=0.0,my=0.0;
    scanf("%d%d",&n,&m);
    for(int i=1;i<=n;i++)
    {
        scanf("%d%d",&x[i],&y[i]);
        mx=mx>x[i]?mx:x[i];//减小二分上界（其实没必要）
        my=my>y[i]?my:y[i];
    }
    //二分平方，保证精度（其实也没必要）
    double l=0.0,r=mx*mx+my*my,mid;
    while(r-l>eps)
    {
        mid=(l+r)/2.0;
        if(check(mid))
            l=mid;
        else
            r=mid;
    }
    printf("%.2lf\n",sqrt(l));
    return 0;
}


//////////////////////////////////////

#include<bits/stdc++.h>
using namespace std;
struct node
{
    int u,v;
    double w;
}a[1000000];
int n,f[10000],cnt,k,l;
int x[10000],y[10000];
void add(int uu,int vv,double ww)  //uu到vv的距离为ww
{a[++cnt].u=uu;a[cnt].v=vv;a[cnt].w=ww;}
bool cmp(node a1,node a2)
{return a1.w<a2.w;}
int find(int u)  //并查集
{
    if(f[u]==u) return u;
    else return f[u]=find(f[u]);
}
int main()
{
    scanf("%d%d",&n,&k);
    for(int i=1;i<=n;i++)
        f[i]=i;
    for(int i=1;i<=n;i++)
        scanf("%d%d",&x[i],&y[i]);
    for(int i=1;i<=n;i++)
        for(int j=i+1;j<=n;j++)   //把任意两个野人的距离存起来
        {
            double s=sqrt((x[i]-x[j])*(x[i]-x[j])+(y[i]-y[j])*(y[i]-y[j]));
            add(i,j,s);
        }
    sort(a+1,a+cnt+1,cmp);     //排序
    for(int i=1;i<=cnt;i++)    
    {
        int uu=find(a[i].u);   
        int vv=find(a[i].v);
        if(uu==vv) continue;   //边连的两点在同一个部落，跳过
        f[uu]=vv;              //连起来
        l++;                  
        if(l==n-k+1)           //第n-k+1条边为答案
        {
            printf("%.2lf",a[i].w);
            return 0;
        }
    }
    return 0;
}
