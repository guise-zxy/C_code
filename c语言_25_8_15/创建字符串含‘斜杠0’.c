#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int main() {
	char arr1[] = "12345\0";          /*��������Ϊ����5���ַ�������Ĭ����ӵ����'\0'Ϊ�ַ������������������ַ������ݵ�һ���֡�*/
	char arr2[] = "12345";
	int set1 = 0;
	int set2 = 0;
	set1= strlen(arr1);         /*������5*/
	set2 = strlen(arr2);		  /*������5*/
	

	return 0;
}