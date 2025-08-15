#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	char arr1[] = "12345\0";          /*编译器认为还是5个字符，他会默认你加的这个'\0'为字符串结束符，而不是字符串内容的一部分。*/
	char arr2[] = "12345";
	int set1 = 0;
	int set2 = 0;
	set1= strlen(arr1);         /*都等于5*/
	set2 = strlen(arr2);		  /*都等于5*/
	

	return 0;
}