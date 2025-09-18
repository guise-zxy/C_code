#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int sum(int n)
{
	if (n < 10)
	{
		return n;
	}
	else
	{
		return sum(n / 10) + n % 10;
	}

}

int main() {
	int n = 0;
	scanf("%d", &n);
	printf("%d", sum(n));



	return 0;
}