#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>


int main() {
	int a = 0;
	scanf("%d", &a);

	for (int i = 1; i <= a; i++) {
		for (int j = 1; j <= i; j++)
		{
			printf("%d*%d=%d\t", j, i, i * j);
		}
		printf("\n");
	}




	return 0;
}