#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
void LeftRound(char a[], int k)
{
	size_t len = strlen(a);
	size_t times = k % len;
	char temp = 0;
	int i = 0;
	for (i = 0; i < times; i++)
	{
		temp = a[0];
		int j = 0;
		for (j = 0; j < len - 1; j++)
		{
			a[j] = a[j + 1];
		}
		a[len-1] = temp;

	}
}

int main()
{
	char a[] = "abcdef";
	int n = 0;
	scanf("%d", &n);
	LeftRound(a, n);
	printf("%s", a);

	return 0;
}