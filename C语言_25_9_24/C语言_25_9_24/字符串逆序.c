#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>

void Reverse(char* str)
{
	char*  left = str;
	char* right =str+ (strlen(str) - 1);
	while (left < right)
	{
		char temp = *right;
		*right = *left;
		*left = temp;
		left++;
		right--;
	}


}


int main() {
	char a[10] = "abcdef";
	Reverse(a);
	printf("%s", a);
	return 0;
}