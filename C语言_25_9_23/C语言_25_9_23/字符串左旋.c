#define _CRT_SECURE_NO_WARNINGS

//#include<stdio.h>
//#include<string.h>
//
//void Roated(char a[], int length,int num)
//{
//	int i = 0;
//	char temp[10] = {0};
//	for (int j = 0; j < num; j++)
//	{
//		temp[j] = a[j];
//	}
//	int kj = num-1,k=0;
//	for (k = 0; k < length-num; k++)    //�Ѳ�����������ǰ�ƣ�
//	{
//		a[k] = a[kj++];
//	}
//
//	for (int o = 0; o < num - 1; o++)   //��Ҫ�������ƶ�������ȥ
//	{
//		a[k++] = temp[o];
//	}
//
//}
//int main() {
//	char a[5] = {};
//	int i = 0;
//	while (1)
//	{
//		char c = getchar();
//		
//		if (c == '\n' || c == EOF)
//		{
//			break;
//		}
//		a[i++] = c;
//	}
//	int n = 0;
//	scanf("%d", &n);
//	Roated(a, 5,n);
//
//	for (int i = 0; i < 4; i++)
//	{
//		printf("%c ", a[i]);
//	}
//	return 0;
//}

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

void Roated(char a[], int length, int num) {
    // ����k�����ַ������ȵ����
    num = num % length;
    if (num == 0) return;

    char temp[256] = { 0 };
    // ����ǰnum���ַ�
    for (int i = 0; i < num; i++) {
        temp[i] = a[i];
    }
    // ��ʣ���ַ�ǰ��
    for (int i = num; i < length; i++) {
        a[i - num] = a[i];
    }
    // ����ʱ�洢���ַ��ŵ��ַ���ĩβ
    for (int i = 0; i < num; i++) {
        a[length - num + i] = temp[i];
    }
}

int main() {
    char a[100] = { 0 };  // Ԥ���㹻�ռ�
    int n = 0;

    printf("�������ַ���: ");
    scanf("%99s", a);  // �������볤�ȷ�ֹ���

    int length = strlen(a);
    printf("�����������ַ���: ");
    scanf("%d", &n);

    Roated(a, length, n);
    printf("��������: %s\n", a);

    return 0;
}