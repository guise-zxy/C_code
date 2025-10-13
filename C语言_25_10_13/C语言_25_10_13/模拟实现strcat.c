#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
char* strcat(char* dest, const char* src)
{
	char* ret = dest;
	//在操作指针之前，判断为空为好习惯！库函数中就会处理这种情况，这里我不做处理；
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