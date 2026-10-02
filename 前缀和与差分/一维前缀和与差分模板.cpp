#include <iostream>

using namespace std;

const int N = 1e6+10;
#define ll long long

ll a[N], sum[N];
int n, m;
int main() {
    cin >> n >> m;
    for (int i = 1; i <= n; ++i) {
        cin >> a[i];
        sum[i] = sum[i - 1] + a[i]; // 前缀合维护部分
    }
    for (int i = 1; i <= m; ++i) {
        int l, r;
        cin >> l >> r;
        cout << sum[r] - sum[l - 1] << '\n';
    }
    return 0;
}
