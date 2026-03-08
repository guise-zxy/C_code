#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
	const double  currency[4] = {6.2619,6.6744,0.0516,0.92};
	double res = 0, num = 0;
	int t;
	char type = 0;
	cin >> t;
	for (int i = 0; i < t; i++)
	{
		cin >> type;
		cin >> num;
		if (type == 'D')res = num * currency[0];
		else if (type == 'E')res = num * currency[1];
		else if (type == 'Y')res = num * currency[2];
		else if (type == 'H')res = num * currency[3];

		cout  << fixed << setprecision(4) << res << endl;


	}
		
		



	return 0;
}