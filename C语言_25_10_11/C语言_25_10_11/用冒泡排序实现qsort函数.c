#define _CRT_SECURE_NO_WARNINGS



#include <stdio.h>
void Swap(char* p1, char* p2,size_t size)
{
	int temp = *(p2 + size);

}

void Qsort(void* s, size_t num, size_t size, int(*hs)(const void* p1, const void* p2))
{
	//�һ��ʵ���������Ƚϵ������㷨�������á�ð������ʵ��
	for (int i = 0; i < num - 1; i++)
	{
		for (int j = 0; j < num - i - 1; j++)
		{   


	//ʵ��һ���ȽϺ���cmp
			if (cmp((char *)s+(j*size), (char*)s + (j + 1)*size) > 0)
			{

	//����������װһ��Swap��������Ԫ��
				Swap((char*)s + (j * size), (char*)s + (j + 1) * size,size);
			}
		}
	}




}


