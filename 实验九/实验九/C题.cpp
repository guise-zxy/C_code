//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <string>
//using namespace std;
//
//class Account {
//private:
//    int account;
//    char type;
//    double sum;
//    double rate;
//
//public:
//    // 普通构造：创建活期账户
//    Account(int acc, char t, double s) {
//        account = acc;
//        type = t;
//        sum = s;
//        rate = 0.005;
//    }
//
//    // 拷贝构造：创建定期账户
//    Account(const Account& a) {
//        account = a.account + 50000000;
//        type = a.type;
//        sum = a.sum;
//        rate = 0.015;
//    }
//
//    void calculateInterest() {
//        sum = sum + sum * rate;
//        cout << "Account=" << account << "--sum=" << sum << endl;
//    }
//
//    void print() {
//        cout << "Account=" << account << "--";
//
//        if (type == 'P') {
//            cout << "Person";
//        }
//        else if (type == 'E') {
//            cout << "Enterprise";
//        }
//
//        cout << "--sum=" << sum << "--rate=" << rate << endl;
//    }
//};
//
//int main() {
//    int t;
//    cin >> t;
//
//    while (t--) {
//        int account;
//        char type;
//        double sum;
//
//        cin >> account >> type >> sum;
//
//        Account current(account, type, sum); // 活期账户
//        Account fixed(current);             // 定期账户，调用拷贝构造
//
//        char op1, op2;
//        cin >> op1 >> op2;
//
//        if (op1 == 'C') {
//            current.calculateInterest();
//        }
//        else if (op1 == 'P') {
//            current.print();
//        }
//
//        if (op2 == 'C') {
//            fixed.calculateInterest();
//        }
//        else if (op2 == 'P') {
//            fixed.print();
//        }
//    }
//
//    return 0;
//}