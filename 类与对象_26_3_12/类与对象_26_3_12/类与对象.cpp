#define _CRT_SECURE_NO_WARNINGS 


#include <stdio.h>
void func1(int x)
{
	/*std::cout << "func(int x)" << '\n';*/
	printf("func(int x)\n");
	return;
}


void func2(int * x)
{
	//std::cout << "func(int * x)" <<'\n';
	printf("func(int * x)\n");
	nullptr;

	return;
}



int main()
{

	//C
	func1((void*)0);


	return 0;
}