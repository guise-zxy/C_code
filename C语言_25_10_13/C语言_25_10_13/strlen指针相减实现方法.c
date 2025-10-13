#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int strlen(const char* str)
{
	char* temp = str;
	
	while (*temp)
	{
		temp++;
	}
	return temp - str;
}


int main()
{

	char a[] = "aefsfsfs";
	printf("%d", strlen(a));
	return 0;
}