#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>

#include<stdio.h>

int main(){
	int* p = (int*)malloc(20);
	
	if(p!=NULL)p[4]; 
	int* p2 = (int*)realloc(p, 40);
                              //此时操作系统重新找了一块空间，然后就马上释放了 p 所指的那块空间，所以在第 12  行时，编译器说没有被初始化；；；；
							  // 而 p 如果放在第8行就可以，因为它此时没被释放；
	if (p2 != NULL)p2[9];
	
	free(p2);
	p2= NULL;


}