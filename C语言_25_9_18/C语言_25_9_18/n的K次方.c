#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int cf(int n, int k)
{
	if (k <= 1)
	{
		return n;
	}
	else
	{
		return cf(n, k - 1) * n;
	}


	
}

int main(){
	int n = 0,k = 0;

	scanf("%d %d", &n, &k);
	printf("%d", cf(n, k));

	return 0;
}