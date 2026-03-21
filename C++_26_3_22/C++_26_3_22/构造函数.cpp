#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
class date
{
public:
	date(date& a, int b = 9)
	{
		x = a.x;
		y = b;
	}

	date(int c, int d)
	{
		x = c;
		y = d;
	}

	void printt()
	{
		std::cout << x << std::endl << y;
		std::cout << std::endl;
	}

private:
	int x;
	int y;
};


int main()
{

	date s1(3, 4);
	date s2(s1,33);
	s1.printt();
	s2.printt();

	return 0;
}