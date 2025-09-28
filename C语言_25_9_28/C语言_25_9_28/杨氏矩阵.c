#define _CRT_SECURE_NO_WARNINGS



#include<stdio.h>
int ysjz(int (*p)[5], int key, int xlen, int ylen)
{
	int x = 0;
	int y = ylen-1;

	while(x<xlen&&y>=0)
	{
	if (p[x][y] < key)
	{
		x++;
	}
	else if (p[x][y] > key)
	{
		y--;
	}
	else
	{
		return 1;
	}
}

	return 0;
}



int main() {
	int a[3][5] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	int n = 0;
	scanf("%d", &n);
	int ret = ysjz(a,n,3,5);
	if (ret == 1)
	{
		printf("找到了");
	}
	else
	{
		printf("没找到");
	}


	return 0;
}