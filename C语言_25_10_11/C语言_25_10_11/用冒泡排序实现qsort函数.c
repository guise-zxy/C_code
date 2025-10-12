#define _CRT_SECURE_NO_WARNINGS



#include <stdio.h>
void Swap(char* p1, char* p2,size_t size)
{
	int temp = *(p2 + size);

}

void Qsort(void* s, size_t num, size_t size, int(*hs)(const void* p1, const void* p2))
{
	//搭建一个实现两两数比较的排序算法，这里用”冒泡排序“实现
	for (int i = 0; i < num - 1; i++)
	{
		for (int j = 0; j < num - i - 1; j++)
		{   


	//实现一个比较函数cmp
			if (cmp((char *)s+(j*size), (char*)s + (j + 1)*size) > 0)
			{

	//满足条件封装一个Swap函数交换元素
				Swap((char*)s + (j * size), (char*)s + (j + 1) * size,size);
			}
		}
	}




}


