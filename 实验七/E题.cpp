#define _CRT_SECURE_NO_WARNINGS

整可提交代码：

#include <iostream>
#include <string>
using namespace std;

class CDate {
private:
    int year, month, day;

public:
    CDate(int y, int m, int d) {
        year = y;
        month = m;
        day = d;
    }

    void print() {
        cout << year << "." << month << "." << day;
    }
};

class Phone {
private:
    char type;       // A机构 B企业 C个人 D备份
    string number;   // 手机号码
    int state;       // 1在用 2未用 3停用
    CDate* stopDate; // 停机日期指针

public:
    Phone(char t, string num, int s);
    Phone(const Phone& p);
    ~Phone();

    void stop(int y, int m, int d);
    void print();
};

Phone::Phone(char t, string num, int s) {
    type = t;
    number = num;
    state = s;
    stopDate = NULL;

    cout << "Construct a new phone " << number << endl;
}

Phone::Phone(const Phone& p) {
    type = 'D';
    number = p.number + "X";
    state = p.state;

    if (p.stopDate == NULL) {
        stopDate = NULL;
    }
    else {
        // 深拷贝，不能直接 stopDate = p.stopDate
        stopDate = new CDate(*p.stopDate);
    }

    cout << "Construct a copy of phone " << p.number << endl;
}

Phone::~Phone() {
    if (stopDate != NULL) {
        delete stopDate;
    }
}

void Phone::stop(int y, int m, int d) {
    state = 3;

    if (stopDate != NULL) {
        delete stopDate;
    }

    stopDate = new CDate(y, m, d);

    cout << "Stop the phone " << number << endl;
}

void Phone::print() {
    cout << "类型=";

    if (type == 'A')
        cout << "机构";
    else if (type == 'B')
        cout << "企业";
    else if (type == 'C')
        cout << "个人";
    else if (type == 'D')
        cout << "备份";

    cout << "||号码=" << number;

    cout << "||State=";
    if (state == 1)
        cout << "在用";
    else if (state == 2)
        cout << "未用";
    else if (state == 3)
        cout << "停用";

    if (stopDate != NULL) {
        cout << "||停机日期=";
        stopDate->print();
    }

    cout << endl;
}

int main() {
    int t;
    cin >> t;

    while (t--) {
        char type;
        string number;
        int state;
        int y, m, d;

        cin >> type >> number >> state >> y >> m >> d;

        Phone p1(type, number, state);
        p1.print();

        Phone p2(p1);
        p2.print();

        p1.stop(y, m, d);
        p1.print();

        cout << "----" << endl;
    }

    return 0;
}