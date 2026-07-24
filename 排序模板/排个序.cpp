#include <bits/stdc++.h>
using namespace std;

const int N = 1010;
int a[N], p[N];
int n, m;

// 检查数组是否非降序
bool isSorted(int a[], int n) {
    for (int i = 1; i < n; ++i) {
        if (a[i] > a[i + 1]) return false;
    }
    return true;
}

int main() {
    scanf("%d %d", &n, &m);
    for (int i = 1; i <= n; ++i) {
        scanf("%d", &a[i]);
    }
    for (int i = 1; i <= m; ++i) {
        scanf("%d", &p[i]);
    }

    bool swapped = true;
    while (swapped) {
        swapped = false;
        for (int i = 1; i <= m; ++i) {
            int pos = p[i];
            if (a[pos] > a[pos + 1]) { // 当前位置需要交换
                swap(a[pos], a[pos + 1]);
                swapped = true;
                if (isSorted(a, n)) { // 每次交换后检查全局有序性
                    printf("YES");
                    return 0;
                }
            }
        }
    }

    printf("NO");
    return 0;
}
