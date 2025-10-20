#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

#include<stdlib.h>





int main() {



	int (*p)[3] = (int (*)[3])malloc(60);

	for (int i = 0; i < 3; i++)

	{

		for (int j = 0; j < 5; j++)

		{

			p[i][j] = i + j;

		}

	}

	for (int i = 0; i < 3; i++)

	{

		for (int j = 0; j < 5; j++)

		{

			printf("%d ", p[i][j]);

		}

	}



	return 0;

}