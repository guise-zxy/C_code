#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
int Int_com(const void* p1, const void* p2)
{

	return (*(int*)p2) - (*(int*)p1);
}
int main() {
	
	int a[7] = { 3,6,45,3,9,8,2 };
	int sz = sizeof(a) / sizeof(a[0]);
	qsort(a,sz, sizeof(int), Int_com);
	for (int i = 0; i < sz; i++) {
		printf("%d ", a[i]);
	}

	return 0;
}