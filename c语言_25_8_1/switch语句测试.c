#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>


int main() {
    int a=0;
    scanf("%d", &a);
    switch (a) {
    case 1: printf("One"); break;
    default: printf("Default");
        break;// 注意这里没有break
    case 2: printf("Two"); 
    }


	return 0;
}