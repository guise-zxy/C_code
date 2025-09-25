#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
size_t strlen(const char* str)
{
	size_t count = 0;
	while (*str != '\0') 
		{
			count++;
			str++;

		}

	return count;
}
int main() {
	char a[] = "abcdeffff";
	size_t ret = strlen(a);
	printf("%zd", ret);



	return 0;
}