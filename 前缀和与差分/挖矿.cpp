#include <bits/stdc++.h>
using namespace std;
int main()
{
  ios::sync_with_stdio(0),cin.tie(0),cout.tie(0);
  int n, m; cin >> n >> m;
  int z = 0;
  vector<int> l(m + 1),r(m + 1);
  for(int i = 0;i < n;i ++)
  {
    int num;cin >> num;
    if(abs(num) <= m && num < 0)l[-num] ++;
    else if(abs(num) <= m && num > 0)r[num] ++;
    else if(num == 0)z ++;
  }
  for(int i = 1;i <= m;i ++)
  {
    l[i] += l[i - 1];
    r[i] += r[i - 1];
  }
  int cnt = max(r[m], l[m]);
  for(int i = 1;i <= m / 2;i ++)
  {
    int back2r = l[i] + r[m - 2 * i];
    int back2l = r[i] + l[m - 2 * i];
    cnt = max(cnt, max(back2l, back2r)); 
  }
  cnt += z; 
  cout << cnt << endl;
  return 0;
}
