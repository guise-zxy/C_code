#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
void* memmove(void* dest, const char* src,size_t num)            //size_t num :所覆盖元素的大小（字节数)
{
	
	char* p = dest;
	if (dest < src)
	{
		while (num--)
		{
			*(char *)dest = *(char*)src;
			dest = (char*)dest + 1;
			src = (char*)src + 1;
		}
	}
	else
	{
		while (num--)
		{
			*(((char*)dest) + num) = *(((char*)src) + num);
			

		}
	}

	return dest;
}

int main() {


	int a[5] = { 1,2,3,4,5 };
	
	memmove(a + 2,a,12 );
		for (int i = 0; i < 5; i++)
	{
		printf("%d ", a[i]);
	}
	return 0;
}