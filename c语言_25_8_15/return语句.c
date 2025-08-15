#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
void test(int n) {
	if (n <= 0) {
		return;
	}
	int i = 0;
	int sum=0;
	for (i = 1; i <= n; i++) {
		sum += i;
	}
	printf("%d", sum);}


int main() {

	test(5);



	return 0;
}