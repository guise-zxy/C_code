#define _CRT_SECURE_NO_WARNINGS                   //�⺯��strcpy�´��src��'\0',���Լ�����������src�ַ���һ��Ҫ��'\0'!!!!
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