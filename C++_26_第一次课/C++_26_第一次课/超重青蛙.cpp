#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
using namespace std;


int main()
{
	int t, n;
	double sum = 0;
	double weight[20] = { 0 };
	double ave = 0;
	double num = 0;
	cin >> t;
	for (int i = 0; i < t; i++)
	{
		cin >> n;
		for (int j = 0; j < n; j++)
		{
			cin >> weight[j];
			sum += weight[j];
		}
		ave = sum / n;
		for (int k = 0; k < n; k++)
		{
			if (weight[k] > ave)
			{
				num++;
			}
		}
		cout << num << endl;
		sum = 0;
		num = 0;


	}


	return 0;
}