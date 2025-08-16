#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
int is_leap_year(int year) {
	if ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

int main() {
	int y = 0;
	int r = 0;
	scanf("%d", &y);
	r = is_leap_year(y); 
		if (r == 1)
		{
			printf("是闰年");

		}
		else
		{
			printf("不是闰年");
		}




	return 0;
}