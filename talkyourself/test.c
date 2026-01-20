#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 第1关：一级难度 - 普通整数加法
void level1() {
    int a, b;
    scanf("%d", &a);
    scanf("%d", &b);
    printf("%d\n", a + b);
}

// 第2关：二级难度 - 大整数加法（最多19位）
void level2() {
    char a[20], b[20];
    scanf("%s", a);
    scanf("%s", b);
    
    // 使用long long存储（可以处理最多19位）
    long long num_a = atoll(a);
    long long num_b = atoll(b);
    printf("%lld\n", num_a + num_b);
}

// 第3关：二级难度2 - 四进制正整数加法
void level3() {
    char a[20], b[20];
    scanf("%s", a);
    scanf("%s", b);
    
    int len_a = strlen(a);
    int len_b = strlen(b);
    int max_len = len_a > len_b ? len_a : len_b;
    
    int result[21] = {0};
    int carry = 0;
    
    for (int i = 0; i < max_len; i++) {
        int digit_a = (i < len_a) ? (a[len_a - 1 - i] - '0') : 0;
        int digit_b = (i < len_b) ? (b[len_b - 1 - i] - '0') : 0;
        int sum = digit_a + digit_b + carry;
        result[i] = sum % 4;
        carry = sum / 4;
    }
    
    if (carry > 0) {
        result[max_len] = carry;
        max_len++;
    }
    
    // 输出结果（四进制）
    for (int i = max_len - 1; i >= 0; i--) {
        printf("%d", result[i]);
    }
    printf("\n");
}

// 第4关：三级难度 - 四进制带符号整数加法
void level4() {
    char a[20], b[20];
    scanf("%s", a);
    scanf("%s", b);
    
    int is_negative_a = (a[0] == '-');
    int is_negative_b = (b[0] == '-');
    
    // 提取数字部分
    char *num_a = is_negative_a ? a + 1 : a;
    char *num_b = is_negative_b ? b + 1 : b;
    
    int len_a = strlen(num_a);
    int len_b = strlen(num_b);
    int max_len = len_a > len_b ? len_a : len_b;
    
    int result[21] = {0};
    int carry = 0;
    
    for (int i = 0; i < max_len; i++) {
        int digit_a = (i < len_a) ? (num_a[len_a - 1 - i] - '0') : 0;
        int digit_b = (i < len_b) ? (num_b[len_b - 1 - i] - '0') : 0;
        int sum = digit_a + digit_b + carry;
        result[i] = sum % 4;
        carry = sum / 4;
    }
    
    if (carry > 0) {
        result[max_len] = carry;
        max_len++;
    }
    
    // 判断结果符号
    if (is_negative_a && is_negative_b) {
        printf("-");
    }
    
    // 输出结果（四进制）
    for (int i = max_len - 1; i >= 0; i--) {
        printf("%d", result[i]);
    }
    printf("\n");
}

// 第5关：四级难度 - 大实数加法（最多100位有效数字）
void level5() {
    char a[105], b[105];
    scanf("%s", a);
    scanf("%s", b);
    
    // 找到小数点位置
    int dot_pos_a = -1, dot_pos_b = -1;
    for (int i = 0; a[i]; i++) {
        if (a[i] == '.') dot_pos_a = i;
    }
    for (int i = 0; b[i]; i++) {
        if (b[i] == '.') dot_pos_b = i;
    }
    
    // 分离整数部分和小数部分
    char int_a[105], frac_a[105], int_b[105], frac_b[105];
    
    if (dot_pos_a == -1) {
        strcpy(int_a, a);
        strcpy(frac_a, "0");
    } else {
        strncpy(int_a, a, dot_pos_a);
        int_a[dot_pos_a] = '\0';
        strcpy(frac_a, a + dot_pos_a + 1);
    }
    
    if (dot_pos_b == -1) {
        strcpy(int_b, b);
        strcpy(frac_b, "0");
    } else {
        strncpy(int_b, b, dot_pos_b);
        int_b[dot_pos_b] = '\0';
        strcpy(frac_b, b + dot_pos_b + 1);
    }
    
    // 对齐小数部分
    int len_frac_a = strlen(frac_a);
    int len_frac_b = strlen(frac_b);
    int max_frac = len_frac_a > len_frac_b ? len_frac_a : len_frac_b;
    
    for (int i = len_frac_a; i < max_frac; i++) frac_a[i] = '0';
    frac_a[max_frac] = '\0';
    for (int i = len_frac_b; i < max_frac; i++) frac_b[i] = '0';
    frac_b[max_frac] = '\0';
    
    // 计算小数部分加法
    char frac_result[105] = {0};
    int carry = 0;
    int pos_frac = 0;
    
    for (int i = max_frac - 1; i >= 0; i--) {
        int sum = (frac_a[i] - '0') + (frac_b[i] - '0') + carry;
        frac_result[pos_frac++] = sum % 10 + '0';
        carry = sum / 10;
    }
    
    frac_result[pos_frac] = '\0';
    
    // 反转小数部分
    for (int i = 0; i < pos_frac / 2; i++) {
        char temp = frac_result[i];
        frac_result[i] = frac_result[pos_frac - 1 - i];
        frac_result[pos_frac - 1 - i] = temp;
    }
    
    // 计算整数部分加法
    int len_int_a = strlen(int_a);
    int len_int_b = strlen(int_b);
    int max_int = len_int_a > len_int_b ? len_int_a : len_int_b;
    
    char int_result[105] = {0};
    int pos_int = 0;
    
    for (int i = 0; i < max_int; i++) {
        int digit_a = (i < len_int_a) ? (int_a[len_int_a - 1 - i] - '0') : 0;
        int digit_b = (i < len_int_b) ? (int_b[len_int_b - 1 - i] - '0') : 0;
        int sum = digit_a + digit_b + carry;
        int_result[pos_int++] = sum % 10 + '0';
        carry = sum / 10;
    }
    
    if (carry > 0) {
        int_result[pos_int++] = carry + '0';
    }
    
    int_result[pos_int] = '\0';
    
    // 反转整数部分
    for (int i = 0; i < pos_int / 2; i++) {
        char temp = int_result[i];
        int_result[i] = int_result[pos_int - 1 - i];
        int_result[pos_int - 1 - i] = temp;
    }
    
    // 输出结果
    printf("%s.%s\n", int_result, frac_result);
}

int main() {
    int level;
    printf("请选择关卡(1-5): ");
    scanf("%d", &level);
    
    switch(level) {
        case 1:
            level1();
            break;
        case 2:
            level2();
            break;
        case 3:
            level3();
            break;
        case 4:
            level4();
            break;
        case 5:
            level5();
            break;
        default:
            printf("无效的关卡选择\n");
    }
    
    return 0;
}
