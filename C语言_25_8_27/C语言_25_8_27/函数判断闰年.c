#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdbool.h>

_Bool hp(int year)
{
	if (year % 400 == 0 || (year % 4 == 0) && (year % 100 != 0))
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

int main() {
	int year = 0;
	scanf("%d", &year);
	if (hp(year))
	{
		printf("是闰年");
	}
	else
	{
		printf("不是");
	}


	
		return 0;
}