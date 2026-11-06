#define _CRT_SECURE_NO_WARNINGS 1
typedef int HPDataType;
#include <stdio.h>
#include <assert.h>
void Swap(int* x, int* y)
{
	int temp = *y;
	*y = *x;
	*x = temp;
	return;
}


//向上调整
void AdjustUp(HPDataType* a, int child)
{
	assert(a);
	int parent = (child - 1) / 2;
int  * p = &parent;
	p = (void*)NULL;

	while (parent >= 0)                             //判断条件
	{
		//建小堆>;建大堆<
		if (a[parent] < a[child])
		{
			Swap(&a[parent], &a[child]);
		}
		else
		{
			break;
		}
		child = parent;
		parent = (child - 1) / 2;
	}

	return;
}

//向下调整
void AdjustDown(HPDataType* a, int num, int parent)
{
	assert(a);
	int child = parent * 2 + 1;    //默认左节点最大；运用用假设法去比较；


	while (child <= num - 1)     //只要左孩子存在就进入循环
	{


		//此处代码为建大堆,假设法比较左孩子与右孩子；而在比较之前得先判断右孩子是否存在，以防数组访问越界
		if (child + 1 <= num - 1 && a[child] < a[child + 1])
		{
			//切换为右节点
			child++;

		}

		//比较parent与child
		if (a[parent] < a[child])
		{
			//交换parent与child
			Swap(&a[parent], &a[child]);
			parent = child;
			child = parent * 2 + 1;
		}
		//提前退出，已经更新完
		else
		{
			break;
		}
	}

	return;
}



//向上调整算法建堆
void HeapSortUp(HPDataType* a,int n)
{

	for (int i = 1; i <= n; i++)
	{
		AdjustUp(a, i);

	}


	return;
}

//向下建堆
void HeapSortDown(HPDataType* a, int n)
{
	for (int i = (n - 1 - 1) / 2;  i >= 0;i--)
	{
		AdjustDown(a, n, i);
	}
}


//堆排序；；；；向下调整建堆，，，堆排序
| void HeapSort(HPDataType * a, int n)
{
	//1.建堆
	//降序，建小堆
	//升序，建大堆
	for (int i = (n - 1 - 1) / 2; i--; i >= 0)
	{
		AdjustDown(a, n, i);
	}


	//将最小或者最大的根节点与当前堆里倒数第一个"叶子节点"调换位置，然后继续向下调整AdjustDown，直至堆里只剩下自己;
	//此时，数组就为有序序列

	int end = n - 1;
	while (end > 0)
	{
		Swap(&a[0],&a[end]);
		AdjustDown(a, end, 0);
		end--;
	}

	//将最小或者最大的根节点与当前堆里倒数第一个"叶子节点"调换位置，然后继续向下整，直至堆里只剩下自己




}



int main()
{
	int a[]={4,2,8,1,5,6,9,7};
	HeapSortUp(a, 8);
	for (int i = 0; i < 8; i++)
	{
		printf("%d ", a[i]);
	}
	return 0;
}