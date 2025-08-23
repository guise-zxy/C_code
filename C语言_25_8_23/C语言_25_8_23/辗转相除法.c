#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>


int main()
{
	int a = 18;
	int b = 24;
	int c = 0;

	while (c = a % b)
	{
		a = b;
		b = c;
	}

	printf("%d\n", b);
	return 0;
}