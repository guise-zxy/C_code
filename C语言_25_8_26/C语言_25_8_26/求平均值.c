#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>


int main() {
	double sum = 0;
	int a[10] = {};
	for (int i = 0; i < 10; i++) {
		scanf("%d", &a[i]);
		sum += a[i];
	}
	printf("%f", sum / 10);




	return 0;
}