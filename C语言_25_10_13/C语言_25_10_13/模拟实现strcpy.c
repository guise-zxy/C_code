#define _CRT_SECURE_NO_WARNINGS                   //库函数strcpy会拷贝src的'\0',所以即参数所产的src字符串一定要有'\0'!!!!
#include<stdio.h>
char * strcpy(char* dest, const char* src)
{
	char* ret = dest;
	while (*src)
	{
		*(dest++) = *(src++);
	}
	*dest = '\0';
	return ret;
}

int main() {
	char sr[6] = "bc";
	char a[7] = "act";
	printf("%s", strcpy(a, sr));




	return 0;
}