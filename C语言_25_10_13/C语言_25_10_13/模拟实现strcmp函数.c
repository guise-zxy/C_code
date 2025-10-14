#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
int   strcmp(const char* p1, const char* p2)
{
	while ((*p1) == (*p2))
	{
		if (*p1 == '\0')
		{
			return 0;
		}
		p1++;
		p2++;

	}
	return *p1 - *p2;
}

#include<stdio.h>
int main()

{
	char a[] = "abcfdr";
	char b[] = "bcfer";
	int ret = strcmp(a, b);
	printf("%d", ret);

	return 0;
}