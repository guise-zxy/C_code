#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int jc(int n)
{
	if (n == 1||n==0)     {             //��д0���Ļ�����ջ�������n==1��ʼ��Ϊ�٣����������޵ݹ���ȥ��!	{
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