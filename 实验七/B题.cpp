#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
using namespace std;


class objectt
{
private:
	int x;



public:
	objectt()
	{
		x = 0;
		cout << "Constructed by default, value = 0\n";
	}

	objectt(int value)
	{
		x = value;
		cout << "Constructed using one argument constructor, value = " << x << '\n';
	}

	objectt(const objectt& p)
	{
		x = p.x;
		cout << "Constructed using copy constructor, value = " << x << '\n';
		
	}


};


int main()
{
	int t = 0,decision=0;
	cin >> t;
	while (t--)
	{

		cin >> decision;
		if (decision == 0)
		{
			objectt p1;
		}
		else if(decision==1)
		{
			int temp = 0;
			cin >> temp;
			objectt p2(temp);
		}
		else
		{
			int temp = 0;
			cin >> temp;
			objectt p3(temp);
			objectt p4 = p3;
		}


	}



	return 0;
}