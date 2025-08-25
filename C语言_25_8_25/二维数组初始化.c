#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int main() {
	int a[][3] = { {0,1,2},{3,4},{} };
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}

	


	return 0;
}