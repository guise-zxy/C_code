#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
using namespace std;


class phone
{
private:
	string a;

public:
	phone()
	{
		this->a = new char[7];
	}

	phone(const phone& p)
	{
		if (p.a[0] == '2' || p.a[0] == '3' || p.a[0] == '4')
		{
			'8' += p.a;
		}
		else if (p.a[0] == '5' || p.a[0] == '6' || p.a[0] == '7' || p.a[0] == '8')
		{
			'2' += p.a;
		}
	}

	~phone()
	{
		delete[]a;
	}
	
	print()
	{
		cout << a;
	}


};


int main()
{
	int t = 0;
	cin >> t;
	while (t--)
	{
		string temp;
		cin >> temp;
		if(temp[0]=='')
		phone obj;
		cin >> obj.a;
	}



	return 0;
}

#include <iostream>
#include <cstring>
using namespace std;

class CTelNumber {
private:
    char* number;

public:
    CTelNumber(char* num);
    CTelNumber(const CTelNumber& t);
    ~CTelNumber();

    void print();
};

// 构造函数：保存原来的7位号码
CTelNumber::CTelNumber(char* num) {
    int len = strlen(num);

    // 判断长度是否为7
    if (len != 7) {
        number = NULL;
        return;
    }

    // 判断是否全是数字
    for (int i = 0; i < len; i++) {
        if (num[i] < '0' || num[i] > '9') {
            number = NULL;
            return;
        }
    }

    // 判断开头是否合法：只能是2~8
    if (num[0] < '2' || num[0] > '8') {
        number = NULL;
        return;
    }

    number = new char[8];   // 7位号码 + '\0'
    strcpy(number, num);
}

// 拷贝构造函数：把7位号码升级成8位号码
CTelNumber::CTelNumber(const CTelNumber& t) {
    if (t.number == NULL) {
        number = NULL;
        return;
    }

    number = new char[9];   // 8位号码 + '\0'

    if (t.number[0] >= '2' && t.number[0] <= '4') {
        number[0] = '8';
    }
    else if (t.number[0] >= '5' && t.number[0] <= '8') {
        number[0] = '2';
    }
    else {
        number = NULL;
        return;
    }

    strcpy(number + 1, t.number);
}

// 析构函数
CTelNumber::~CTelNumber() {
    if (number != NULL) {
        delete[] number;
    }
}

// 输出
void CTelNumber::print() {
    if (number == NULL) {
        cout << "Illegal phone number" << endl;
    }
    else {
        cout << number << endl;
    }
}

int main() {
    int t;
    cin >> t;

    while (t--) {
        char num[100];
        cin >> num;

        CTelNumber oldTel(num);      // 构造原7位号码
        CTelNumber newTel(oldTel);   // 拷贝构造，升位成8位号码

        newTel.print();
    }

    return 0;
}