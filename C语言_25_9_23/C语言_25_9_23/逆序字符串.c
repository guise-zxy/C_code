#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <string.h>

int main() {
    char arr[10010];
    int i = 0;

    // ��ȡ�ַ�ֱ���������з���EOF
    while (1) {
        int c = getchar();
        if (c == '\n' || c == EOF) {
            break;
        }
        arr[i++] = c;
    }
    arr[i] = '\0'; // �ֶ�����ַ�����ֹ��

    size_t sz = strlen(arr);
    for (int i = sz - 1; i >= 0; i--) {
        printf("%c", arr[i]);
    }
    printf("\n"); // ������з���������ĿҪ��

    return 0;
}

	