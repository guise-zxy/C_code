#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int main() {
	int sum = 1;
	for (int i = 10; i <= 100; i++)
	{
		
		
			int ls = i;
			while (ls)
			{
				if (ls% 9 == 0)
				{
					sum++;
				}
				 ls/= 10;
			}
		



	}
	printf("%d", sum);
	return 0;
}