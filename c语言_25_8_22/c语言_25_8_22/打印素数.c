#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

int main() {
	for (int i = 100; i <= 200; i++) {
		int flag = 1;
		for (int j = 2; j < i; j++)
		{
			if (i % j == 0)
			{
				flag = 0;
				break;
			}
		}
		if (flag)
		{
			printf("%d ", i);
		}


	}




	return 0;
}



