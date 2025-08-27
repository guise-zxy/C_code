#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
void init(int arr[],int n)
{
	for (int i = 0; i < n; i++)
	{
		arr[i] = 0;
	}
}

void print(int arr[],int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n============\n");
}
void reverse(int arr[],int n) 
{	
	int temp[100] = { 0 };
	for (int i = 0; i < n; i++)
	{
		temp[i] = arr[i];
	}
	for (int i = n - 1, j = 0; i >= 0 && j < n; i--, j++)
	{
		arr[j] = temp[i];
	}


	
}


int main() {
	int arr[100] = {1,0};
	print(arr,100);
	reverse(arr, 100);
	print(arr,100);
	return 0;
}