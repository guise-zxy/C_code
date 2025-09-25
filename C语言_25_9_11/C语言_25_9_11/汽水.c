#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
int numcan(int money)
{
	return money + (money / 2);
}

int main() {
	int money = 0;
	scanf("%d", &money);
	printf("%d\n", numcan(money));



	return 0;
}