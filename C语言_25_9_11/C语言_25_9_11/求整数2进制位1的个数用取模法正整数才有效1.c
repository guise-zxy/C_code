#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
//这种方法只限正整数才有效，负数就不有效了。比如"-1"就不行了，count等于0；
int main() {
	int n = 0;
	scanf("%d", &n);
	int count = 0;
	while (n)
	{
		if (n % 2 == 1)
		{
			count++;
		}
		n /= 2;

	}
	printf("%d\n", count);






	return 0;
}