#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main() {
    int a, b;
    scanf("%d %d", &a, &b);
    int arr1[1000] = { 0 };
    int arr2[1000] = { 0 };
    int i = 0;
    int j = 0;
    for (int i = 0; i < a; i++) {
        scanf("%d", &arr1[i]);
    }

    for (int j = 0; j < b; j++) {
        scanf("%d", &arr2[j]);
    }


    while (i < a && j < b)
    {
        if (arr1[i] > arr2[j])
        {
            printf("%d ", arr2[j]);
            j++;
        }
        else
        {
            printf("%d ", arr1[i]);
            i++;
        }

    }

    while (i < a)
    {
        printf("%d ", arr1[i]);
        i++;
    }

    while (j < b)
    {
        printf("%d ", arr2[j]);
        j++;
    }



    return 0;
}
