#define _CRT_SECURE_NO_WARNINGS
void Printbit(int num)
{
	for (int i = 31; i >= 1; i -= 2
		)
	{
		printf("%d ", (num >> i) & 1
		);
	}
	printf("\n"
	);

	for (int i = 30; i >= 0; i -= 2
		)
	{
		printf("%d ", (num >> i) & 1
		);
	}
	printf("\n"
	);
}