#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

int main() {
	int max= -2147483648LL;
	int current = 0;
	for (int j = 1; j <= 10; j++) {
		scanf("%d", &current);
		if (current > max)
		{
			max = current;
		}


	}

	printf("%d", max);






	return 0;
}