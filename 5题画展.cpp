using i64=long long;

void mktx()
{
    int n,m;
    cin>>n>>m;
    vector<int>a(n+1);
    for(int i=1;i<=n;i++) cin>>a[i];
    sort(a.begin()+1,a.end());
    i64 res=1e18;
    for(int i=1;i+m-1<=n;i++) res=min(res,1ll*a[i+m-1]*a[i+m-1]-1ll*a[i]*a[i]);
    cout<<res;
}






#include <bits/stdc++.h>
using namespace std;
using i64 = long long;

#define endl '\n'
#define all(v) v.begin(), v.end()

void sol()
{
    int n, m; cin >> n >> m;
    vector<int> A(n);
    vector<i64> B(n);
    for (int i = 0; i < n; i++) {
        cin >> A[i];
        B[i] = 1LL * A[i] * A[i];
    }

    sort(all(B));

    i64 ans = LLONG_MAX;
    for (int i = m - 1; i < n; i++) {
        ans = min(ans, B[i] - B[i - m + 1]);
    }

    cout << ans << endl;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    sol();
    return 0;
}


