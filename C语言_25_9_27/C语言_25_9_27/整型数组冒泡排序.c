#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

void Bubble_sort(int* b, int len)
{
	int j = 0;
	for (j = 0; j <len-1; j++)
	{
		int i = 0;   
		for (i = 0; i < len - 1-j; i++)
		{
			if (b[i] < b[i + 1])
			{
				int temp = b[i + 1];
				b[i + 1] = b[i];
				b[i] = temp;
			}
		}
	}
}

void print(int a[], int len)
{
	for (int i = 0; i < len; i++)
	{
		printf("%d ", a[i]);
	}
}



int main() {
	int a[] = { 1,5,3,4,12,3,5,7,8 };
	int sz = sizeof(a) / sizeof(a[0]);
		Bubble_sort(a, sz);
	print(a, sz);


	return 0;
}