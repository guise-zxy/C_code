#include <iostream>
#include<vector>
#include<algorithm>
using namespace std;

//定义结构体，存储左右区间端点
struct Node{
    int a;
    int b;
};
vector<Node> nv;//全局变量，nv存储各个区间

//自定义比较函数
bool ngreater(Node& n1,Node& n2)
{
    return n1.b<n2.b;//右区间从小到大排序
}

//判断函数

int check(int mid)
{
    int mr=0;//maxright 最大右区间
    vector<Node> temp(nv);
    while(1)//可能有特殊区间，多试几次
    {
        int flag=0;
        for(int i=0;i<temp.size();i++)
        {
            int na=temp[i].a,nb=temp[i].b;
            int len=nb-na;
            if(na-mid<=mr&&nb+mid>=mr)//满足这个条件的区间才是可以通过左右移动和mr连接并且产生新的mr的区间
            {
                flag=1;//说明在这一次遍历所有区间里，可以扩大mr
                if(na+mid>=mr)
                    mr+=len;
                else
                    mr=nb+mid;
                temp.erase(temp.begin()+i);//用过的区间就去掉，不能重复考虑
                break;//不能少 每一个往后面连接的区间都从右端点最小的区间找
            }
        }
        if(mr>=2e4||flag==0)//0到2e4已经被覆盖了 或者是剩下的区间不能再扩大mr了 就退出while
            break;
    }
    if(mr>=2e4)
        return 1;
    else
        return 0;
}

int main()
{
    int n;
    cin>>n;
    int a,b;
    for(int i=0;i<n;i++)
    {
        cin>>a>>b;
        nv.push_back({a*2,b*2}); 
    }
    sort(nv.begin(),nv.end(),ngreater);//排序是因为每次尽可能选择最右端小的区间相连，这样尽可能让每一个区间移动距离最小。
    //贪心策略吧
    int l=0,r=2e4,mid;//左闭右闭  扩大两倍是为了计算浮点数保留小数
    //每一个区间都可以向左或者向右移动大小不超过最大区间位移量的位移量，最大的区间位移量越大，就越容易覆盖整个区间
    while(l<=r)
    {
        mid=(l+r)>>1;
        if(check(mid))//这个假定的最大区间位移mid是可以覆盖0到1e4的，那么就可以尝试更小的区间移位
            r=mid-1;
        else//这个最大区间位移mid太小了，不能够覆盖，就要换更大的区间位移
            l=mid+1;
    }
    double result=((double)l)/2;//二分结束后，l=r+1,要以最后一个mid可以的值作为答案，此时l=mid，而r=mid-1
    cout<<result<<endl;
    return 0;
}
