#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>

int main() {
    int T;  // ����ʵ������
    scanf("%d", &T);

    while (T--) {
        int n;  // ��������
        scanf("%d", &n);

        float weights[100];  // �洢��������
        float total = 0.0;   // �����ܺ�
        float average;       // ƽ������
        int count = 0;       // �������ܼ���[2](@ref)

        // ��ȡ���ز������ܺ�
        for (int i = 0; i < n; i++) {
            scanf("%f", &weights[i]);
            total += weights[i];
        }

        // ����ƽ������
        average = total / n;

        // ͳ�Ƴ�����������
        for (int i = 0; i < n; i++) {
            if (weights[i] > average) {
                count++;
            }
        }

        // ������
        printf("%d\n", count);
    }

    return 0;
}