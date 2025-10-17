#define _CRT_SECURE_NO_WARNINGS



#include <stdio.h>
//�á�ָ�롱ʵ�� Swap����
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
	//�һ��ʵ���������Ƚϵ������㷨�������á�ð������ʵ��
	for (int i = 0; i < num - 1; i++)
	{
		for (int j = 0; j < num - i - 1; j++)
		{

			
			//�ȽϺ���cmp
			if (cmp((char*)s + (j * size), (char*)s + (j + 1) * size) > 0)
			{

				//����������װһ��Swap��������Ԫ��
				Swap((char*)s + (j * size), (char*)s + (j + 1) * size, size);
			}
		} 
	}


}

//ʵ��cmp����
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