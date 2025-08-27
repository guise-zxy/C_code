#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int bin_search(int arr[], int left, int right, int key) {
    while (left <= right) {
        int middle = left + (right - left) / 2;  // ��ֹ����������ȼ��� (left+right)/2��������ȫ��
        if (arr[middle] == key) {
            return middle;  // �ҵ�Ŀ�꣬�����±�
        }
        else if (arr[middle] > key) {
            right = middle - 1;  // Ŀ������벿�֣��ų� middle ���Ҳ�
        }
        else {
            left = middle + 1;   // Ŀ�����Ұ벿�֣��ų� middle �����
        }
    }
    return -1;  // δ�ҵ�Ŀ��
}

int main() {
    int arr1[5] = { 1, 5, 6, 6, 7 };
    int result = bin_search(arr1, 0, 4, 8);  // ����8�������ڣ�
    printf("%d", result);
    return 0;
}

// arr �ǲ��ҵ�����
//left ��������±�
//right ��������±�
//key Ҫ���ҵ�����