#define _CRT_SECURE_NO_WARNINGS
#include "heap.h"



//交换函数
void Swap(HPDataType* x, HPDataType* y)
{
	HPDataType temp = 0;
	temp = *x;
	*x = *y;
	*y = temp;

	return;
 }


//初始化
void HPInit(HP* php)
{
	assert(php);
	php->a = NULL;
	php->capacity = php->size = 0;
	return;
}



//销毁
void HPDestroy(HP* php)
{
	assert(php);
	free(php->a);
	php->a = NULL;
	php->size = php->capacity = 0;

}



//向上调整
void AdjustUp(HPDataType* a, int child)
{
	assert(a);
	int parent = (child - 1) / 2;

	while (parent>=0)                             //判断条件
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
void AdjustDown(HPDataType* a,int num, int parent)
{
	assert(a);
	int child = parent * 2 + 1;    //默认左节点最大；运用用假设法去比较；
	
	
	while (child<=num-1)     //只要左孩子存在就进入循环
	{


		//此处代码为建大堆,假设法比较左孩子与右孩子；而在比较之前得先判断右孩子是否存在，以防数组访问越界
		if (child+1<=num-1&&a[child] <a[child + 1] )
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


////插入
//void HPPush(HP* php, HPDataType x)
//{
//	assert(php);
//	assert(php->a);
//
//	//增容
//	if (php->capacity == php->size)  
//	{
//		int newcapacity = (php->capacity == 0) ? 4 : 2 * php->capacity;
//		php->capacity = newcapacity;
//
//	}
//
//	php->size++;
//	//向上调整
//	AdjustUp(php->a, php->size-1);
//
//
//
//	return;
//}


//插入
void HPPush(HP* php, HPDataType x)
{
	assert(php);
	
	//不能添加这句，因为heap初始化时php->a为NULL;所有php->NULL是有效情况，即heap没元素时
	//assert(php->a);

	//增容
	if (php->capacity == php->size)
	{
		int newcapacity = (php->capacity == 0) ? 4 : 2 * php->capacity;


	//realloc更新数组地址
		HPDataType* tmp = (HPDataType*)realloc(php->a,newcapacity*sizeof(HPDataType));
		//注意：动态申请空间时，一定要检查返回的地址
	
		/*	perror(tmp);*/
		if (tmp == NULL)
		{
			perror("realloc failed!\n");
			return;
		}
	
		php->a = tmp;
		php->capacity = newcapacity;

	}

	//插入节点
	php->a[php->size] = x;
	php->size++;
	//向上调整
	AdjustUp(php->a, php->size - 1);

	return;
}







//删除
void HPPop(HP* php)
{
	assert(php);
	assert(php->a);

   //1.交换根节点与最后一个叶子点。
     Swap(&php->a[0], &php->a[php->size - 1]);
	
  //2.逻辑上删除最后一个节点,更新size。。。此步骤一定得在3步之前；否则相当于"堆顶元素"未删除，参与向下调整。
	php->size--;

	//3.向下调整
	AdjustDown(php->a,php->size,0);

	
	return;
}

 //返回堆顶数据
HPDataType  HPTop(HP * php)
{
	assert(php);
	assert(php->size>0);
	return php->a[0];
}

//判空
_Bool IsEmpty(HP * php)
{
	assert(php);
	return php->size == 0;
}



//向上调整算法建堆
void HeapSortUp(HPDataType* a, int n)
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
	for (int i = (n - 1 - 1) / 2; i--; i >= 0)
	{
		AdjustDown(a, n, 0);
	}
}


//堆排序



| voi d HeapSort(HPDataType * a, int n)
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



//求叶子节点
 root->left--NULL && root->right == NULL ? 1 : TreeSize(root->left) + TreeSize(roo->right);