#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
char* strcat(char* dest, const char* src)
{
	char* ret = dest;
	//�ڲ���ָ��֮ǰ���ж�Ϊ��Ϊ��ϰ�ߣ��⺯���оͻᴦ����������������Ҳ�������
	while (*(dest))
	{
		dest++;
	}
	while (*(src))
	{
		*(dest) = *(src);
		src++;
		dest++;
	}
	*(dest) = '\0';
	return ret;
}

int main()
{
	char a[20] = "acexxxxxxx";
	char b[20] = "bgi";
	printf("%s", strcat(b,a));

	return 0;
}