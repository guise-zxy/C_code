#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <assert.h>


typedef int HPDataType;


typedef struct Heap //控制堆即完全二叉树的结构体
{
	HPDataType* a;
	int size;
	int capacity;
}HP;



void HPInit(HP * php);//初始化
void HPDestroy(HP* php);//销毁


//插入
