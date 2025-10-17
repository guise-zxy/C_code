#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
int  pd()
{ 
int a =3; //0003
	char* p = &a;

	if (*p == 3)return 1;
	else return 0;
}
int main()
{
	printf("%d", pd());
}
