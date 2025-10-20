#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#define offsetof(p,n) (&n-p)                       //p为结构体变量的地址
                                                   //n为要求偏移量的结构体成员变量


int main() {


	struct s
	{
		int i;
		char c;
	}s1;

	printf("%d", offsetof(&s1, s1.c));                      //传s1的地址    和      要求偏移量的成员变量；
	                                                                     
	return 0;
}