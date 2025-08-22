#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
void gcd(int a, int b) {
	if (a % b == 0)
	{
		printf("%d", b);
			return;
	}
	else
	{
		gcd(a % b, a);
	}



}
int main() {
	int a = 0;
	int b = 0;
	scanf("%d %d", &a, &b);
	gcd(a, b);



	return 0;
}