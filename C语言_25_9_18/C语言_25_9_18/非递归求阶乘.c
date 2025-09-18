#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int main() {
	int n = 0;
	scanf("%d", &n);
	int i = 1;
	unsigned long long sum = 1;

	for (i = 1; i <= n; i++)
	{
		sum *= i;
	}
	printf("%lld", sum);



	return 0;
}