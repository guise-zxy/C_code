#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main()
{
	int n = 0;
	scanf("%d", &n);
	int sn = n;
	int temp = n;
	for (int i = 0; i < 4; i++)
	{
		temp=temp* 10+n;
		sn += temp ;
	
	}
	printf("%d", sn);



	return 0;
}