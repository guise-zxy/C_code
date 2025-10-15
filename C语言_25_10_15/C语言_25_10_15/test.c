#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

extern int c;
 extern int mul(int x, int y);
 
int main() {

	int ret = 0;
	ret = mul(9 ,4);
	printf("%d ", ret);
	
	printf("%d", c);
	return 0;
}