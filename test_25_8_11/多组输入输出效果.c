#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

int main() {
    int a, b;
    while (scanf("%d", &b) != EOF) { // ע�� while ������ case
        // 64 λ������� printf("%lld") to 
        while (b != 0) {
            printf("*");
            b--;
        }
        printf("\n");
    }
    return 0;
}