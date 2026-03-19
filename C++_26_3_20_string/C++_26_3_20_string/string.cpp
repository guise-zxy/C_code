#define _CRT_SECURE_NO_WARNINGS




#include <iostream>
#include <string>
using namespace std;
//class string
//{
//	string(const string& s)
//	{
//
//	}
//private:
//	char *
//	int capacity = 0;
//	int size = 0;
//
//};


int main()
{
	char a[5] = "ertr";
	string s0;
	string s1("abcceeesc defgdf");      //
	string s2(s1,  2);                  //Subsequence
	string s3(s1, 1, 4);                //Substring
	string s4(4, 'f');
	string s5(a);
	 
	//第七种：迭代器写法

	cout << s0<< endl;
	cout << s1 << endl;
	cout << s2 << endl;
	cout << s3<< endl;
	cout << s4 << endl;
	cout << s5 << endl;
	return 0;
}