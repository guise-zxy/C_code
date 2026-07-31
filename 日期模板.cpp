#include <stdio.h>

// 每个月的日期数,ds[i]代表i月份的天数
int ds[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int main() {
    int sy = 2022; // 起始年份
    int ey = 2022; // 结束年份
    int week = 6; // 定义 0-6 ,0为星期日，1为星期一，2为星期二...
    int ans = 0;
    for (int y = sy; y <= ey; ++y) { // 第一层循环，枚举年份
        for (int m = 1; m <= 12; ++m) { // 第二层循环，枚举月份
            int dd = ds[m];
            if (y % 4 == 0 && y % 100 != 0 && m == 2) // 判断是不是闰月
                dd = 29;
            else if (y % 400 == 0 && m == 2) // 判断是不是闰月
                dd = 29;
            for (int d = 1; d <= dd; ++d) { // 枚举天
                if (week == 0 || week == 6 || d % 10 == 1) { // 如果满足某一个条件，就记录答案
                        ans ++;
                    }
                week = (week + 1) % 7; // 向后推移星期几
            }
            
        }
    }
    printf("%d\n", ans);
    return 0;
}
