#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
void menu() {
	printf("********\n");
	printf("*1.play*\n");
	printf("*0.exit*\n");
	printf("********\n");


}

void game() {
	int guess = 0;
	//���������
	srand((unsigned int)time(NULL));
	int ret = rand()%100+1;
	
	while(1)
	{ scanf("%d", &guess);

	if (guess > ret)
	{
		printf("�´���");

	}
	else if (guess < ret)
	{
		printf("��С��");
	}
	else
	{
		printf("�¶���");
		break;
	}
	
	}
	

}

int main() {
	int input = 0;
	do 
	{
		menu();
		printf("��ѡ��");
		
		scanf("%d", &input);
			switch (input)
			{
			case 1:
				printf("������\n");
				game();
				break;
			case 0:
				printf("�˳���Ϸ\n");
				break;
			default:
				printf("ѡ���������ѡ��");
					break;
			}
	} while (input);




	return 0;
}