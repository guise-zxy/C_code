#include <bits/stdc++.h>
using namespace std;				//暴力遍历 
int q[2010][2010];
int sum[2010][2010];
int n,m;
int x1,y1,x2,y2;
int d[2010][2010];
int main()
{
  scanf("%d %d",&n,&m); 

for(int i=1;i<=m;i++){
  scanf("%d %d %d %d",&x1,&y1,&x2,&y2);
  for(int j=x1;j<=x2;j++){
    for(int k=y1;k<=y2;k++){
      if((q[j][k]&1)==1)q[j][k]=0;
      else{
        q[j][k]=1;
      }
    }
  }
}

for(int i=1;i<=n;i++){
  for(int j=1;j<=m;j++){
    printf("%d",q[i][j]);
    }
    printf("\n");
  }

  // 请在此输入您的代码
  return 0;
}



///////////////////////
#include<bits/stdc++.h>
using namespace std ;
#define ll long long				///差分数组 
const int N = 2e3+9 ;
int d[ N ][ N ] , n , m ;
int main(){
    cin >> n >> m ;
    for( int i = 1 ; i <= m ; i ++ ){
        int x1 , y1 , x2 , y2 ; cin >> x1 >> y1 >> x2 >> y2 ;
        d[ x1 ][ y1 ] ++ ;
        d[ x1 ][ y2+1 ] -- ;
        d[ x2+1 ][ y1 ] -- ;
        d[x2+1][y2+1]++ ;
    }

    for( int i = 1 ; i <= n ; i ++ ) {
        for( int j = 1 ; j <= n; j++ ){
            d[i][j] = (d[i][j] + d[i-1][j] + d[i][j-1] - d[i-1][j-1]) ;
            if( d[i][j]&1 ) cout << "1" ;
            else cout << "0" ;
        }
        cout << "\n" ;
    }
    return 0 ;
}



