#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

char* strncpy(char* dest,const char* src,size_t n)
{
	char* s = dest;
	while (n--)
	{

		if (*src == '\0')
		{
			*dest = '\0';
			dest++;
		}
		else
		{
			*dest = *src;
			dest++;
			src++;
		}
	}
	return s;
}

int main() {
	char a[100] = "a";
	char b[] = "bcd";

	printf("%s", strncpy(a, b, 2));

	return 0;
}