#define _CRT_SECURE_NO_WARNINGS

//向上调整算法建堆
void HeapSortUp(HPDataType * a ,int n)
{

	for (int i = 1; i <= n; i++)
	{
		AdjustUp(a, i);

	}


	return;
}

//向下建堆
void HeapSortDown(HPDataType* a,int n)
{
	for (int i = (n - 1 - 1) / 2; i--; i >= 0)
	{
		AdjustDown(a, n, 0);
	}
}


//堆排序



| void HeapSort(HPDataType * a,int n)
{
	//1。建堆
	//降序，建小堆
	//升序，建大堆
	for (int i = (n - 1 - 1) / 2; i--; i >= 0)
	{
		AdjustDown(a, n, 0);
	}


	//将最小或者最大的根节点与当前堆里倒数第一个"叶子节点"调换位置，然后继续向下整，直至堆里只剩下自己;
	//此时，数组就为有序序列

	int end = n - 1;
	while (end > 0)
	{
		AdjustDown(a, n, 0);
	}

	//将最小或者最大的根节点与当前堆里倒数第一个"叶子节点"调换位置，然后继续向下整，直至堆里只剩下自己
	 



}