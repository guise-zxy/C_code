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
			printf("������");

		}
		else
		{
			printf("��������");
		}




	return 0;
}