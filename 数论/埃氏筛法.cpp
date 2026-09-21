#include<bits/stdc++.h>
using namespace std;

const int MAX = 10000010;
bool isprime[MAX]; // 初始化为false（全局变量默认初始化）
int ans = 0;

int main() {
    fill(isprime, isprime + MAX, true); // 所有数初始化为素数
    isprime[0] = isprime[1] = false;     // 0和1不是素数

    for (int i = 2; i*i< MAX; ++i) {
        if (isprime[i]) { // 发现素数
            ans++;
            if (ans == 100002) {
                printf("%d\n", i);
                return 0;
            }
            // 从i*2开始标记所有倍数为非素数
            for (int j = i * i; j < MAX; j += i) {
                isprime[j] = false;
            }
        }
    }
    return 0;
}
