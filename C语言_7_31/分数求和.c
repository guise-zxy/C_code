#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<math.h>  

int main() {
    double sum = 0;
    int i = 1;
    for (i = 0; i <= 99; i++) {
        sum += pow(-1, i) * (1.0 / (i + 1));  // pow����(-1)��i�η�
    }
    printf("%lf\n", sum);

    return 0;
}