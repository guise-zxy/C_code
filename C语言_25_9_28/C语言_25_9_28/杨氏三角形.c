#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>
void ysmatrix(int a[][4], int n)
{

	int i = 0;
	int j = 0;
	for(i=0;i<n;i++)
	{
		for (j = 0; j <=i; j++)
		{

	
		if (i == j || j == 0)
		{
			a[i][j] = 1;
		}
		else
		{
			a[i][j] = a[i - 1][j - 1] + a[i - 1][j];
		}
		}
}



}


int main() {
	int matrix[4][4] = { 0 };

	ysmatrix(matrix, 4);

	for (int i = 0; i < 4; i++)
	{
		for (int j =0; j <=i; j++)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}






}