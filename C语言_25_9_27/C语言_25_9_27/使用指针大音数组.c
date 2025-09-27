#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>
void print(int b[],int sz) {
	
	int i = 0;
	for (i = 0; i < sz; i++)
	{
		printf("%d ", *(b + i));
	}



	return;
}


int main() {
	int a[] = { 1,2,3,4,5,6,3 ,7};
	
	int sz = sizeof(a) / sizeof(int);

print(a,sz);

	return 0;
}