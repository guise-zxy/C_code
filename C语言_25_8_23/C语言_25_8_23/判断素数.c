#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>  // ���� sqrt ��������
////����4
///*
//�����Է������Ż���ֻҪi����[2, sqrt(i)]֮����κ�������������i������������ʵ���ڲ���ʱi���ô�101�𽥵�����200����Ϊ����2��3֮�⣬�����������������ڵ�����ͬʱΪ����

int main() {
    int i = 0;
    int count = 0;

    for (i = 101; i <= 200; i += 2) {  // ����101~200������
        int is_prime = 1;  // ����Ƿ�Ϊ������1��ʾ�ǣ�0��ʾ��
        // �Գ��� sqrt(i)���� j*j <= i �����
        for (int j = 2; j * j <= i; j++) {
            if (i % j == 0) {  // ���ܱ�j��������������
                is_prime = 0;
                break;  // ��ǰ�˳�ѭ��
            }
        }
        if (is_prime) {  // �������Ϊ1��������
            count++;
            printf("%d ", i);
        }
    }

    printf("\ncount = %d\n", count);
    return 0;
}