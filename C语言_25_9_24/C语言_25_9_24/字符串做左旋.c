#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>


int main() {
		int n = 0, k = 0;
		scanf("%d %d", &n, &k);

		//读取字符串；
		int i = 0;
		while (getchar() != '\n');
		char a[10] = { 0 };
		for (i = 0; i < n; i++)                 
		{
			char c = getchar();
			if (c != '\n')
			{
				a[i] = c;
				
			}
			else
			{
				break;
			}
		}
		a[n] = '\0';


		//开始左旋；
		
		//准备临时数组 temp,将要左旋的元素放入临时数组
		char temp[20] = { 0 };
		for (int i = 0; i < k; i++)
		{
			temp[i] = a[i];
		}


		//将剩余元素前置
		for (int i = k; i < n; i ++ )
		{
			a[i - k] = a[i];
		}

		//将temp数组的元素放回后面
		int j = 0;
		for (j = 0; j < k; j++)
		{
			a[n - k + j] = temp[j];
		}
		a[n - k + j] = temp[j];
		printf("%s", a);
		return 0;
}