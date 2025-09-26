#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>

int main() {
    int T;  // 测试实例数量
    scanf("%d", &T);

    while (T--) {
        int n;  // 青蛙数量
        scanf("%d", &n);

        float weights[100];  // 存储青蛙体重
        float total = 0.0;   // 体重总和
        float average;       // 平均体重
        int count = 0;       // 超重青蛙计数[2](@ref)

        // 读取体重并计算总和
        for (int i = 0; i < n; i++) {
            scanf("%f", &weights[i]);
            total += weights[i];
        }

        // 计算平均体重
        average = total / n;

        // 统计超重青蛙数量
        for (int i = 0; i < n; i++) {
            if (weights[i] > average) {
                count++;
            }
        }

        // 输出结果
        printf("%d\n", count);
    }

    return 0;
}