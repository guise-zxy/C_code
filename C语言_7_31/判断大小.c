#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	int a = 0;
	int b = 0;
	int c = 0;

	scanf("%d %d %d", &a, &b, &c);
	/*�ж����*/
	if (a > b)
	{
		if (a > c)
		{
			printf("%d", a);
		}
		else
		{
			if (b > c)
			{
				printf("%d", b);

			}
			else {
				printf("%d", c);
			}
		}
	}
	else
	{
		if (b > c)
		{
			printf("%d", b);

		}
		else
		{
			printf("%d", c);

		}
	}

	/*�ж��м���*/
	if ((a <= b && b <= c) || (c <= b && b <= a))
	{
		printf(" %d", b);

	}
	else if ((c <= a && a <= b) || (b <= a && a <= c))
	{
		printf(" %d", a);
	}
	else
	{
		printf(" %d", c);
	}


	/*�ж���С*/
	if (a < b)
	{
		if (a < c)
		{
			printf(" %d", a);
		}
		else
		{
			if (b < c)
			{
				printf(" %d", b);

			}
			else {
				printf(" %d", c);
			}
		}
	}
	else
	{
		if (b < c)
		{
			printf(" %d", b);

		}
		else
		{
			printf(" %d", c);

		}
	}


	return 0;
}