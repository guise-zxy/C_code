#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>



int main()
{
	
	for (char killer = 'a'; killer <= 'd'; killer++)
	{
		if (((killer != 'a') + (killer == 'c') + (killer == 'd') + (killer != 'd'))==3)
		{
			printf("Ð×ÊÖÊÇ%c", killer);
		}
		
	}



	return 0;
}