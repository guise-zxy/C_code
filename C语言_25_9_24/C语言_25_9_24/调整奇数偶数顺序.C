#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
void Reverse(int a[],int n)
{
	int p = 0, q = 1;
	            
}

int main() {
	int n = 0;
	int a[100] = { 0 };
	scanf("%d", &n);
	int i = 0;
	for (i = 0; i < n; i++)
	{
		scanf("%d", &a[i]);
	}


	Reverse(a,n);

	return 0;
}