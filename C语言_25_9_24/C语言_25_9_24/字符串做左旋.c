#define _CRT_SECURE_NO_WARNINGS


#include<stdio.h>


int main() {
		int n = 0, k = 0;
		scanf("%d %d", &n, &k);

		//��ȡ�ַ�����
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


		//��ʼ������
		
		//׼����ʱ���� temp,��Ҫ������Ԫ�ط�����ʱ����
		char temp[20] = { 0 };
		for (int i = 0; i < k; i++)
		{
			temp[i] = a[i];
		}


		//��ʣ��Ԫ��ǰ��
		for (int i = k; i < n; i ++ )
		{
			a[i - k] = a[i];
		}

		//��temp�����Ԫ�طŻغ���
		int j = 0;
		for (j = 0; j < k; j++)
		{
			a[n - k + j] = temp[j];
		}
		a[n - k + j] = temp[j];
		printf("%s", a);
		return 0;
}