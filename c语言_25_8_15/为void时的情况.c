#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
void print(void) {       /*形参”void“可以省略*/
	printf("hello");
}

int main() {
	print();
	return 0;
}