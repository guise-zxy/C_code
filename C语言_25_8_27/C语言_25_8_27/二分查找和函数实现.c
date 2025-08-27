#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int bin_search(int arr[], int left, int right, int key) {
    while (left <= right) {
        int middle = left + (right - left) / 2;  // 防止整数溢出（等价于 (left+right)/2，但更安全）
        if (arr[middle] == key) {
            return middle;  // 找到目标，返回下标
        }
        else if (arr[middle] > key) {
            right = middle - 1;  // 目标在左半部分，排除 middle 及右侧
        }
        else {
            left = middle + 1;   // 目标在右半部分，排除 middle 及左侧
        }
    }
    return -1;  // 未找到目标
}

int main() {
    int arr1[5] = { 1, 5, 6, 6, 7 };
    int result = bin_search(arr1, 0, 4, 8);  // 查找8（不存在）
    printf("%d", result);
    return 0;
}

// arr 是查找的数组
//left 数组的左下标
//right 数组的右下标
//key 要查找的数字