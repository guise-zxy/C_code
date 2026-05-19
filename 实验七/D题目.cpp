#include <iostream>
#include <cstring>
using namespace std;

class CDate
{
private:
    int year, month, day;

public:
    CDate(int y, int m, int d);
    bool isLeapYear();
    int getYear();
    int getMonth();
    int getDay();
    int getDayofYear();
};

CDate::CDate(int y, int m, int d)
{
    year = y;
    month = m;
    day = d;
}

bool CDate::isLeapYear()
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int CDate::getYear()
{
    return year;
}

int CDate::getMonth()
{
    return month;
}

int CDate::getDay()
{
    return day;
}

int CDate::getDayofYear()
{
    int sum = day;
    int a[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (isLeapYear())
        a[2]++;

    for (int i = 0; i < month; i++)
        sum += a[i];

    return sum;
}

class Software
{
private:
    char name[31];
    char type;
    char media;
    CDate endDate;

public:
    Software(char n[], char t, char m, int y, int mon, int d);
    Software(const Software& s);

    int getRemainDays();
    void print();
};

Software::Software(char n[], char t, char m, int y, int mon, int d)
    : endDate(y, mon, d)
{
    strcpy(name, n);
    type = t;
    media = m;
}

Software::Software(const Software& s)
    : endDate(s.endDate)
{
    strcpy(name, s.name);
    type = 'B';
    media = 'H';
}

int Software::getRemainDays()
{
    int y = endDate.getYear();
    int m = endDate.getMonth();
    int d = endDate.getDay();

    int days1 = 365 * (2015 - 1)
        + (2015 - 1) / 4
        - (2015 - 1) / 100
        + (2015 - 1) / 400
        + CDate(2015, 4, 7).getDayofYear();

    int days2 = 365 * (y - 1)
        + (y - 1) / 4
        - (y - 1) / 100
        + (y - 1) / 400
        + endDate.getDayofYear();

    return days2 - days1;
}

void Software::print()
{
    cout << "name:" << name << endl;

    cout << "type:";
    if (type == 'O')
        cout << "original" << endl;
    else if (type == 'T')
        cout << "trial" << endl;
    else if (type == 'B')
        cout << "backup" << endl;

    cout << "media:";
    if (media == 'D')
        cout << "optical disk" << endl;
    else if (media == 'H')
        cout << "hard disk" << endl;
    else if (media == 'U')
        cout << "USB disk" << endl;

    if (endDate.getYear() == 0 && endDate.getMonth() == 0 && endDate.getDay() == 0)
    {
        cout << "this software has unlimited use" << endl;
    }
    else
    {
        int days = getRemainDays();

        if (days < 0)
            cout << "this software has expired" << endl;
        else
            cout << "this software is going to be expired in "
            << days << " days" << endl;
    }

    cout << endl;
}

int main()
{
    int t;
    cin >> t;

    while (t--)
    {
        char name[31];
        char type, media;
        int year, month, day;

        cin >> name;
        cin >> type >> media >> year >> month >> day;

        Software s1(name, type, media, year, month, day);
        Software s2(s1);

        s1.print();
        s2.print();
    }

    return 0;
}