#define _CRT_SECURE_NO_WARNINGS
#include<assert.h>
#include<stdio.h>
#include<ctype.h>
int my_atoi( const char a[])
{

	assert(a != NULL);
	if (*a == '\0')return 0;

	int ret = 0;
	int flag = 1;
	while (isspace(*a))                                      //1.先处理“空格”
	{
		a++;
	}

	if (*a == '-' || *a == '+')                            //2.再处理"+,-"号
	{
		if ((*a) == '-')
		{
			flag = -1;
		}
		else
		{
			flag = 1;
		}
			a++;
		
	}

	while (*a)
	{

		if (isdigit(*a))
		{
			ret=ret*10+ (*a - '0')*flag;                      // 处理负数：所以公式应该为：ret=ret*10+(*a='0)*flag;   比如假如是“-45”
			a++;
		}
		else
		{
			return ret;
			
		}

	}
	return ret;

}

int main() {
	char b[] = "      -8aa79";
	printf("%d", my_atoi(b));

	return 0;
}