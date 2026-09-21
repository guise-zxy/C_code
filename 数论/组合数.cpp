#include<bits/stdc++.h>
using namespace std;

int t, n, m;

// 正确实现组合数递归公式，添加边界条件
int c(int a, int b) {
    if (b == 0 || b == a) return 1;   // C(a,0)=1 且 C(a,a)=1
    if (a < b || b < 0) return 0;   // a < b 或 b 负数时返回0
    return c(a-1, b) + c(a-1, b-1); // 修正递归公式
}

int main() {
    scanf("%d", &t);
    while(t--) {
        scanf("%d %d", &n, &m);
        printf("%d\n", c(n, m)); // 统一使用 printf 避免格式问题
    }
    return 0;
}
