#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>
int check()
{
	union re {
		char i;
		int c;
	}s;

	s.c = 1;
	return s.i;

}

int main() {
	int ret = 0;
	if (check()) {
		printf("Ð¡¶Ë");

	}
	

	return  0;
}