#define _CRT_SECURE_NO_WARNINGS 1
#include "heap.h"



void testheap1()
{
	HP heap;
	HPInit(&heap);//³õÊ¼»¯
	HPPush(&heap,4);
	HPPush(&heap,2);
	HPPush(&heap,8);
	HPPush(&heap,1);
	HPPush(&heap,5);
	HPPush(&heap,6);
	HPPush(&heap,9);
	HPPush(&heap,7);

	HPPop(&heap);
	printf("%d ", ( & heap)->a[0]);
	HPPop(&heap);
	printf("%d ", (&heap)->a[0]);

	HPPop(&heap);
	printf("%d ", (&heap)->a[0]);

	HPPop(&heap);
	printf("%d ", (&heap)->a[0]);

	HPPop(&heap);
	printf("%d ", (&heap)->a[0]);

	HPPop(&heap);
	printf("%d ", (&heap)->a[0]);


}
int main() 
{

	testheap1();

	return 0;
}