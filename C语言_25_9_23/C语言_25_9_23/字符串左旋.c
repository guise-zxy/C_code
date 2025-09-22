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
//	for (k = 0; k < length-num; k++)    //把不用左旋的向前移；
//	{
//		a[k] = a[kj++];
//	}
//
//	for (int o = 0; o < num - 1; o++)   //把要左旋的移动到后面去
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
    // 处理k大于字符串长度的情况
    num = num % length;
    if (num == 0) return;

    char temp[256] = { 0 };
    // 保存前num个字符
    for (int i = 0; i < num; i++) {
        temp[i] = a[i];
    }
    // 将剩余字符前移
    for (int i = num; i < length; i++) {
        a[i - num] = a[i];
    }
    // 将临时存储的字符放到字符串末尾
    for (int i = 0; i < num; i++) {
        a[length - num + i] = temp[i];
    }
}

int main() {
    char a[100] = { 0 };  // 预留足够空间
    int n = 0;

    printf("请输入字符串: ");
    scanf("%99s", a);  // 限制输入长度防止溢出

    int length = strlen(a);
    printf("请输入左旋字符数: ");
    scanf("%d", &n);

    Roated(a, length, n);
    printf("左旋后结果: %s\n", a);

    return 0;
}