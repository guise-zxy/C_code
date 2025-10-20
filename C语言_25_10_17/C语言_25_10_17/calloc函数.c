#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<stdlib.h>



int main() {


	int* p = (int*)calloc(4, sizeof(int));
	for (int i = 0; i < 4; i++)
	{
		p[i] = i;
	}
	for (int i = 0; i < 4; i++)
	{
		printf("%d ", p[i]);
	}
	free(p);
	p = NULL;

	return 0;
}