#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	int a = 0;
	while (scanf("%d", &a) != -1)
	{
		for (int i = 0; i < a; i++)
		{
			for (int j = 0; j < a; j++)
			{
				if (i == 0|| i == a - 1 || j == 0 || j == a - 1)
				{
					printf("* ");
				}
				else
				{
					printf("  ");
				}

			}
			printf("\n");
		}
	}

	return 0;
}