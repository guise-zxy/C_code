#define _CRT_SECURE_NO_WARNINGS



#include <stdio.h>
//用“指针”实现 Swap交换
void Swap(void* p1, void* p2, size_t size)
{
	for (int i = 0; i < size; i++)
	{
		char temp = *((char *)p2 + i);
		*((char *)p2 + i) = *((char *)p1 + i);
		*((char * )p1 + i) = temp;
	}

}





void qsort(void* s, size_t num, size_t size, void (*hs)(const void* p1, const void* p2))
{
	//搭建一个实现两两数比较的排序算法，这里用”冒泡排序“实现
	for (int i = 0; i < num - 1; i++)
	{
		for (int j = 0; j < num - i - 1; j++)
		{

			
			//比较函数cmp
			if (cmp((char*)s + (j * size), (char*)s + (j + 1) * size) > 0)
			{

				//满足条件封装一个Swap函数交换元素
				Swap((char*)s + (j * size), (char*)s + (j + 1) * size, size);
			}
		} 
	}


}

//实现cmp函数
int cmp(const void* p1, const void* p2)
{

	return (*(int*)p1) - (*(int*)p2);
}


int main() {
	int a[6] = { 4,3,1,2,0,-1 };
	int sz = sizeof(a) / sizeof(a[0]);
	qsort(a, sz, sizeof(int), cmp);

	for (int k = 0; k < sz; k++)
	{
		printf("%d ", a[k]);
	}
	return 0;
}


#define _CRT_SECURE_NO_WARNINGS