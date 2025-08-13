#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

int main() {
    int a, b;
    while (scanf("%d", &b) != EOF) { // 注意 while 处理多个 case
        // 64 位输出请用 printf("%lld") to 
        while (b != 0) {
            printf("*");
            b--;
        }
        printf("\n");
    }
    return 0;
}