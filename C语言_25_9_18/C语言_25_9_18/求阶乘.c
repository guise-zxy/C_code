#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int jc(int n)
{
	if (n == 1||n==0)     {             //不写0！的话，会栈溢出，（n==1）始终为假，函数会无限递归下去。!	{
		return 1;
	}
	else
	{
		return n * jc(n - 1);
	}
}
int main() {
	int n = 0;
	scanf("%d", &n);
	printf("%d", jc(n));


	return 0;
}