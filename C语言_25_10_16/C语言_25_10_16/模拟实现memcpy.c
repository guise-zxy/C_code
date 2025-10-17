#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
void* memcpy(void* dest, void* src, size_t size)
{
	if (src == NULL || dest == NULL) return;
	int i = 0;
	char* t1 = (char*)dest;
	char * t2 = (char*)src;
	for (i = 0; i < size; i++)
	{
		*(t1 + i) = *(t2 + i);
	}

	return dest;
}

int main() {

	//²âÊÔ¹¦ÄÜ´úÂë
	
	char a[100] = "abdhss";
	char b[] = "fkfhcc";

	printf("%s", memcpy(a, b, 2));

	return 0;
}
