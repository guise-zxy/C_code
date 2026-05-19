//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <string>
//using namespace std;
//
//class PhoneNumber {
//private:
//    int number;
//    char type;
//
//public:
//    PhoneNumber(int n = 0, char t = 'C') {
//        number = n;
//        type = t;
//    }
//
//    int getNumber() {
//        return number;
//    }
//
//    char getType() {
//        return type;
//    }
//
//    void setNumber(int n) {
//        number = n;
//    }
//
//    void setType(char t) {
//        type = t;
//    }
//};
//
//class Phone {
//private:
//    PhoneNumber phoneNumber;
//    int state;
//    string owner;
//
//public:
//    Phone(int num, char type, int st, string name)
//        : phoneNumber(num, type) {
//        state = st;
//        owner = name;
//
//        cout << phoneNumber.getNumber() << " constructed." << endl;
//    }
//
//    ~Phone() {
//        cout << phoneNumber.getNumber() << " destructed." << endl;
//    }
//
//    int query(int num) {
//        if (phoneNumber.getNumber() == num) {
//            return 1;
//        }
//        return 0;
//    }
//
//    void print() {
//        cout << "Phone=" << phoneNumber.getNumber()
//            << "--Type=" << phoneNumber.getType()
//            << "--State=";
//
//        if (state == 1) {
//            cout << "use";
//        }
//        else {
//            cout << "unuse";
//        }
//
//        cout << "--Owner=" << owner << endl;
//    }
//};
//
//int main() {
//    int num1, num2, num3;
//    char type1, type2, type3;
//    int state1, state2, state3;
//    string owner1, owner2, owner3;
//
//    cin >> num1 >> type1 >> state1 >> owner1;
//    cin >> num2 >> type2 >> state2 >> owner2;
//    cin >> num3 >> type3 >> state3 >> owner3;
//
//    Phone p1(num1, type1, state1, owner1);
//    Phone p2(num2, type2, state2, owner2);
//    Phone p3(num3, type3, state3, owner3);
//
//    int t;
//    cin >> t;
//
//    while (t--) {
//        int q;
//        cin >> q;
//
//        if (p1.query(q)) {
//            p1.print();
//        }
//        else if (p2.query(q)) {
//            p2.print();
//        }
//        else if (p3.query(q)) {
//            p3.print();
//        }
//        else {
//            cout << "wrong number." << endl;
//        }
//    }
//
//    return 0;
//}