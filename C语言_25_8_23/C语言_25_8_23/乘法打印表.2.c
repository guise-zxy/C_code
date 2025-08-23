#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main()
{
	int i = 0;
	//控制行数
	for (i = 1; i <= 9; i++)
	{
		//打印每一行内容，每行有i个表达式
		int j = 0;
		for (j = 1; j <= i; j++)
		{
			printf("%d*%d=%-2d ", i, j, i * j);
		}
		printf("\n");
	}
	return 0;
}