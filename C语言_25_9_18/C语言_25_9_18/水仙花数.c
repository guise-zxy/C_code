#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>

#include<math.h>
int main() {
	int i = 0;
	printf("0 ");
	for (int i = 1; i <= 100000; i++){

		int ws = 0;
	int sum = 0;
	int temp1 = i;
	while (temp1)
	{


		ws++;
		temp1 /= 10;

	}
	int temp2 = i;
	while (temp2)
	{
		sum += (int)pow((temp2 % 10), ws);
		temp2 /= 10;
	}
	if (sum == i)
	{
		printf(" %d", sum);
	}
}



	return 0;
}