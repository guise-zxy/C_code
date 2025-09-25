#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>


int main()
{
	char a[18] = "      *      ";

	int j = 6;

	for (int i = 0; i < 7; i++)
	{
		a[j - i] = '*';
		a[j + i] = '*';
		printf("%s\n", a);
	}
	char b[14] = "*************";
	int i = 0, p = 12;
	for (int k = 0; k < 6; k++)
	{	
		b[i] = ' ';
		b[p] = ' ';
		printf("%s\n", b);
		i++, p--;
	
		
	}





	return 0;
}