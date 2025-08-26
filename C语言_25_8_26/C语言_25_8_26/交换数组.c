#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int main() {
	int a[5] = {};
	int b[5] = {  };
	int c[5] = {};
	for (int i = 0; i < 5; i++) {
		scanf("%d", &a[i]);
	}


	for (int i = 0; i < 5; i++) {
		scanf("%d", &b[i]);
	}

	for (int i = 0; i < 5; i++) {
		c[i] = a[i];
		a[i] = b[i];
		b[i] = c[i];
	}

	for (int i = 0; i < 5; i++) {
		printf("%d ", a[i]);
	}
	printf("\n");

	for (int i = 0; i < 5; i++) {
		printf("%d ", b[i]);
	}
	return 0;
}