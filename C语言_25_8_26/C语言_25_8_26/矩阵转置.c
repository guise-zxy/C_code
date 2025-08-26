#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main() {
    int jz[11][11] = {};
    int a, b;
    scanf("%d %d", &a, &b);
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            scanf("%d", &jz[i][j]);

        }
    }
    for (int j = 0; j < b; j++) {
        for (int i = 0; i < a; i++) {
            
                printf("%d ", jz[i][j]);
        }
        printf("\n");
    }
    return 0;
}