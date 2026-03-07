#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
using namespace std;

int main()
{
    char c = 0;
    char a[81] = { 0 };  // 调整为81，确保能容纳80个字符及结束符
    // 读取待查找字符（可能包含空格）
    cin.get(c);
    // 忽略第一行剩余的字符（包括换行符），确保不影响下一行读取
    cin.ignore(1000, '\n');
    // 读取第二行字符串（可包含空格）
    cin.getline(a, 81);

    int index = -1;  // 初始化未找到
    // 遍历字符串，记录字符最后出现的位置
    for (int i = 0; a[i] != '\0'; i++) {
        if (a[i] == c) {
            index = i;
        }
    }

    // 根据结果输出
    if (index != -1) {
        cout << "index = " << index << endl;
    }
    else {
        cout << "Not Found" << endl;
    }

    return 0;
}