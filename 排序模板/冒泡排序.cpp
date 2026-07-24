#include<bits/stdc++.h>
int j; 
void bubbleSortUnoptimized(int arr[], int n) {
	
    for (int i = 0; i < n - 1; ++i) { // 外层循环控制遍历次数
        for (int j = 0; j < n - 1 - i; ++j) { // 内层循环每次减少比较次数
            if (arr[j] > arr[j + 1]) { // 如果前一个元素大于后一个
                swap(arr[j], arr[j + 1]); // 交换两者位置
            }
        }
    }
}






int main(){
	


return 0;
}
