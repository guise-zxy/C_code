#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>



char* strncat(char* dest, const char* src,size_t n )
{
	char* p = dest;
	if (dest == NULL || src == NULL)return NULL ;
	else
	{

		while (*dest++);
		dest = dest - 1;
		int cnt = 0;
		while (*src&&cnt!=n)
		{
			*dest = *src;
			dest++;
			src++;
			cnt++;
		}
		*dest = '\0';             //或者直接    *dest=*src;
	}

	return p;
}

int main() {
	
	char a[100] = "abwfwd";
	char b[] = "fefjjhh";
	char* mb = strncat(a, b,4);
	if (mb != NULL)
	{
		printf("%s",mb);
	}
	else
	{
		printf("为空，操作失败！");
	}

	return 0;
}