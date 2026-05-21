#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <string>
using namespace std;

class Account
{
private:
    static float count;        // 账户总余额
    static float interestRate; // 利率

    string accno;              // 账号
    string accname;            // 姓名
    float balance;             // 当前账户余额

public:
    Account(string ac, string na, float ba)
    {
        accno = ac;
        accname = na;
        balance = ba;
        count += balance;
    }

    ~Account()
    {
        count -= balance;
    }

    void deposit(float amount)
    {
        balance += amount;
        count += amount;
    }

    void withdraw(float amount)
    {
        balance -= amount;
        count -= amount;
    }

    float getBalance()
    {
        return balance;
    }

    void show()
    {
        cout << accno << " " << accname << " " << balance;
    }

    static float getCount()
    {
        return count;
    }

    static void setInterestRate(float rate)
    {
        interestRate = rate;
    }

    static float getInterestRate()
    {
        return interestRate;
    }

    friend void update(Account& a);
};

float Account::count = 0;
float Account::interestRate = 0;

void update(Account& a)
{
    float interest = a.balance * Account::interestRate;
    a.balance += interest;
    Account::count += interest;

    cout << a.balance;
}

int main()
{
    float rate;
    cin >> rate;

    Account::setInterestRate(rate);

    int n;
    cin >> n;

    Account** accounts = new Account * [n];

    for (int i = 0; i < n; i++)
    {
        string accno, accname;
        float balance, depositMoney, withdrawMoney;

        cin >> accno >> accname >> balance >> depositMoney >> withdrawMoney;

        accounts[i] = new Account(accno, accname, balance);

        accounts[i]->deposit(depositMoney);

        accounts[i]->show();
        cout << " ";

        update(*accounts[i]);
        cout << " ";

        accounts[i]->withdraw(withdrawMoney);
        cout << accounts[i]->getBalance() << endl;
    }

    cout << Account::getCount() << endl;

    for (int i = 0; i < n; i++)
    {
        delete accounts[i];
    }

    delete[] accounts;

    return 0;
}