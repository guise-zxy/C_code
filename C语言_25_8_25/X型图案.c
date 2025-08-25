#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
char b[20][20] = {'\0'};
int main() {
    int a = 0;
	
    scanf("%d", &a);
	int i = 0;
	int j = 0;
	for ( ; i < a && j < a; i++,j++) {
		b[i][j] = '*';
		printf("%c", b[i][j]);
		printf("\n");
	}
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < a; j++) {
			printf("%c ", b[i][j]);
		}
	}


    return 0;
}