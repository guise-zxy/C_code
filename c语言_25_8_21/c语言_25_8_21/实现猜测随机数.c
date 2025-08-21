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
	//生成随机数
	srand((unsigned int)time(NULL));
	int ret = rand()%100+1;
	
	while(1)
	{ scanf("%d", &guess);

	if (guess > ret)
	{
		printf("猜大了");

	}
	else if (guess < ret)
	{
		printf("猜小了");
	}
	else
	{
		printf("猜对了");
		break;
	}
	
	}
	

}

int main() {
	int input = 0;
	do 
	{
		menu();
		printf("请选择");
		
		scanf("%d", &input);
			switch (input)
			{
			case 1:
				printf("猜数字\n");
				game();
				break;
			case 0:
				printf("退出游戏\n");
				break;
			default:
				printf("选择错误，重新选择");
					break;
			}
	} while (input);




	return 0;
}