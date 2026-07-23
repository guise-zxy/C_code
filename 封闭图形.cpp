#include <bits/stdc++.h>
using namespace std;
int a[200010],c[200100];
int n;
vector<int>j(100);
int b[10]={1,0,0,0,1,0,1,0,2,1};
int main()
{
    scanf("%d",&n);
    for(int i=1;i<=n;i++){
      scanf("%d",&a[i]);
    }
    for(int i=1;i<=n;i++){
      int sum=0;
      int num=a[i];
        while(num>0){
          sum+=b[num%10];
          num/=10;
        }
        c[i]=sum;
    }

    for(int j=1;j<=n;j++){
  for(int i=1;i<=n-j;i++){
    if(c[i]>c[i+1]){
      swap(a[i],a[i+1]);
    }
    if(c[i]==c[i+1]){
      if(a[i]>a[i+1])swap(a[i],a[i+1]);
    }
  }
}

    for(int i=1;i<=n;i++){
      if(i==1)printf("%d",a[i]);
      else printf(" %d",a[i]);
    }



  
  return 0;
}



