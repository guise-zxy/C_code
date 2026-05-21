#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
using namespace std;

class Hotel
{
private:
    static int totalCustNum;     // 顾客总人数
    static float totalEarning;   // 旅店总收入
    static float rent;           // 每个顾客的房租

    char* customerName;          // 顾客姓名
    int customerId;              // 顾客编号

public:
    static void SetRent(float r)
    {
        rent = r;
    }

    Hotel(char* customer)
    {
        totalCustNum++;

        customerId = 20150000 + totalCustNum;
        totalEarning += rent;

        customerName = new char[strlen(customer) + 1];
        strcpy(customerName, customer);
    }

    ~Hotel()
    {
        delete[] customerName;
    }

    void Display()
    {
        cout << customerName << " "
            << customerId << " "
            << totalCustNum << " "
            << totalEarning << endl;
    }
};

int Hotel::totalCustNum = 0;
float Hotel::totalEarning = 0;
float Hotel::rent = 0;

int main()
{
    float r;
    cin >> r;

    Hotel::SetRent(r);

    char name[100];

    while (cin >> name)
    {
        if (strcmp(name, "0") == 0)
        {
            break;
        }

        Hotel customer(name);
        customer.Display();
    }

    return 0;
}