#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>  // 包含 sqrt 函数声明
////方法4
///*
//继续对方法三优化，只要i不被[2, sqrt(i)]之间的任何数据整除，则i是素数，但是实际在操作时i不用从101逐渐递增到200，因为出了2和3之外，不会有两个连续相邻的数据同时为素数

int main() {
    int i = 0;
    int count = 0;

    for (i = 101; i <= 200; i += 2) {  // 遍历101~200的奇数
        int is_prime = 1;  // 标记是否为素数（1表示是，0表示否）
        // 试除到 sqrt(i)（用 j*j <= i 替代）
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0) {  // 若能被j整除，不是素数
                is_prime = 0;
                break;  // 提前退出循环
            }
        }
        if (is_prime) {  // 若标记仍为1，是素数
            count++;
            printf("%d ", i);
        }
    }

    printf("\ncount = %d\n", count);
    return 0;
}