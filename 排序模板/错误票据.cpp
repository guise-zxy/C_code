#include <bits/stdc++.h>
using namespace std;
   int a, b;
int main() {
    vector<int> v;
    int n; cin >> n;
    for (int i = 0; i < n; i++) {
        int tp;
        while (cin >> tp) {
            v.push_back(tp);
            if (cin.get() == '\n') break;
        }
    }
    sort(v.begin(), v.end());
 
    for (int i = 1; i < v.size(); i++) {
        if (v[i] == v[i-1] + 2)
            a = v[i-1] + 1;
        if (v[i] == v[i-1])
            b = v[i]; 
    }
    cout << a << " " << b;
    return 0;
}
