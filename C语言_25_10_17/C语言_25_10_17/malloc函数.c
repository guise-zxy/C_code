#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>

#include<stdlib.h>

int main() {



	int* p = (int*)malloc(25);
	
	if (p == NULL) {
		printf("ƒ⁄¥Ê∑÷≈‰ ß∞‹£°\n");
		return 1;  // ÷’÷π≥Ã–Ú
	}
	else
	{

	
	for (int i = 0; i < 5; i++)
	{
		*(p + i) = 4;
	}
	for (int i = 0; i < 5; i++)
	{
		printf("%d ", p[i]);
	}
	}


	free(p);
	p = NULL;
	return 0;
}