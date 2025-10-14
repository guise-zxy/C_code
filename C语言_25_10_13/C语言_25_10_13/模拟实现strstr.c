#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
char* strstr(const char* str1, const char* str2)
{
	char* s1 = str1;
	char* s2 = str2;
	char* cur = s1;

	//处理空字符串""->'\0'.
	if (*s2 == '\0')
	{
		return s1;
	}
	while (*cur)
	{

		s1 = cur;
		s2 = str2;
		while ((*s1 != '\0' && (*s2) != '\0') && (*s1) == (*s2))
		{
			s1++;
			s2++;
			if ((*s2) == '\0')
				{
				return cur;
				}

		}
		cur++;

	}


	return NULL;



}

int main()
{
	char a[] = "andjcie";
	char b[] = "";
	if (strstr(a, b) != NULL)
	{
		printf("%s", strstr(a, b));
	}
	else
	{
		printf("不含子串");
	}
	return 0;
}