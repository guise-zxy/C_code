#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
//���ַ���ֻ������������Ч�������Ͳ���Ч�ˡ�����"-1"�Ͳ����ˣ�count����0��
int main() {
	int n = 0;
	scanf("%d", &n);
	int count = 0;
	while (n)
	{
		if (n % 2 == 1)
		{
			count++;
		}
		n /= 2;

	}
	printf("%d\n", count);






	return 0;
}