#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

int main() {

	int n = 0;
	
	int a = 1;
	int b = 1;
	int c = 2;
	scanf("%d", &n);
	if (n == 1 || n == 2)
	{
		printf("1");
	}
	else
	{

	
		for (int i = 1; i <= n-2; i++)
		{
			c = a + b;
			a = b;
			b = c;
		
		}

		printf("%d", c);

    }

	return 0;
}