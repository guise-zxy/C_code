#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef int HPDataType;


typedef struct Heap //控制堆即完全二叉树的结构体
{
	HPDataType* a;
	int size;
	int capacity;
}HP;


void AdjustUp(HPDataType* a, int child); //向上调整
void AdjustDown(HPDataType* a, int num, int parent); //向下调整

void HPInit(HP * php);//初始化
void HPDestroy(HP* php);//销毁
void HPPush(HP* php,HPDataType x); //插入

void HPPop(HP * php);  //删除 

HPDataType  HPTop(HP* php); //返回堆顶数据                            //传heap，还是传数组???



_Bool IsEmpty(HP* php);
