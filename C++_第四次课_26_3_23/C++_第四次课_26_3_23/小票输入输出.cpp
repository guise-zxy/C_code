#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <climits>
using namespace std;
//TianHong 00001 01 CCB 21/06 6029071012345678 2016/3/13 1000.00；商家名称、终端号、操作员、发卡方、有效期、卡号、交易时间、消费金额
struct bills
{
public:

	char s[30];  //商家名称
	char id[30];  //终端号
	char name[30];  //操作员
	char give_card[30];//发卡方
	char eff[30]; //有效期
	char card_id[30]; //卡号
	char time[30];    //交易时间
	char cash[30];    //交易金额



};


int main()
{
	int t = 0;
	cin >> t;
	while (t--)
	{
		bills dx;
		cin >> dx.s >> dx.id >> dx.name >> dx.give_card >> dx.eff >> dx.card_id >> dx.time >> dx.cash;
		char numlef[5] = { dx.card_id[0],dx.card_id[1] ,dx.card_id[2] ,dx.card_id[3] ,'\0' };
		char nummid[9] = "********";
		char numrig[5] = { 0 };
		int k = 0;
		for (int j = 12; j <16; j++)
		{
			numrig[k++] = dx.card_id[j];
		}
		cout << "Name: " << dx.s << "\n";
		cout << "Terminal: " << dx.id <<" operator: "<< dx.name << "\n";
		cout << "Card Issuers: " << dx.give_card << " " << "Validity:" << dx.eff << "\n";
		cout << "CardNumber: " << numlef<<nummid<<numrig<<"\n";
		
		cout << "Traded: " << dx.time << "\n";
		cout << "Costs: $" << dx.cash << "\n";
		cout << "\n";

	}


	return 0;
}