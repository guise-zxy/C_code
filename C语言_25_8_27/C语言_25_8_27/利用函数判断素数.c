#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int is_prime(int x)
{
	if (x <= 1)
	{
		return 0;
	}
	int flag = 1;
	for (int i = 2; i * i <= x; i++) {
		if (x % i == 0)
		{
			flag = 0;
			break;
		}

	}
	if (flag)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

int main() {

	
	//int sum = 0;
	for (int i = 100; i <= 200; i++)
	{
		
		if (is_prime(i))
		{
			printf("%d ", i);
			//sum++;            21¸ö
		}
	}
	//printf("%d", sum);
	

	return 0;
}