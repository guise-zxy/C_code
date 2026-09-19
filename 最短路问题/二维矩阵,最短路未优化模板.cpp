#include<bits/stdc++.h>
using namespace std;
const int N=510,M=1e5+10;
const int INF=0x3f3f3f3f;
int n,m;
int vis[N];
int d[N];
int g[N][N];
int dijkstra()
{
    memset(d,0x3f,sizeof(d));
    d[1]=0;
    for(int i=0;i<n;i++)
    {
        int t=-1;
        for(int j=1;j<=n;j++)
        {
            if(!vis[j]&&(t==-1||d[t]>d[j]))
            {
                t=j;
            }
        }
        if(t==-1||d[t]==INF)
        {
            break;
        }
        vis[t]=1;
        for(int j=1;j<=n;j++)
        {
            d[j]=min(d[j],d[t]+g[t][j]);
        }
    }
    if(d[n]==INF)
    {
        return -1;
    }
    return d[n];
}
void solve()
{
    cin>>n>>m;
    memset(g,0x3f,sizeof(g));
    while(m--)
    {
        int a,b,w;
        cin>>a>>b>>w;
        g[a][b]=min(g[a][b],w);
    }
    int ans=dijkstra();
    cout<<ans;
}
int main()
{
    solve();
}

