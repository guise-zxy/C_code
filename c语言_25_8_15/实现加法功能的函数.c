#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int Add(int x, int y) {
	int z = 0;              /*==>return x+*/
	z = x + y;
	return z;
}
int main() {
	int c = 0;
	int a = 6; 
	int b = 0;
	scanf("%d*%d", &a, &b);
	c = Add(a, b);
	printf("%d", c);


	return 0;
}