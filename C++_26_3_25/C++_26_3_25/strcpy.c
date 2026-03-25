#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>


int main()
{
	char* p = "pppppdpeeeff";
	char a[20] = { 0 };
	strcpy(p, a);
	printf("%s",p );


	return 0;
}