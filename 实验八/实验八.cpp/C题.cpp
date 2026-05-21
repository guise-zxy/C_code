//#define _CRT_SECURE_NO_WARNINGS
//
//#include <iostream>
//#include <iomanip>
//using namespace std;
//
//class Date;
//class Time;
//
//void display(const Date& d, const Time& t);
//
//class Date
//{
//private:
//    int year, month, day;
//
//public:
//    Date(int y, int m, int d)
//    {
//        year = y;
//        month = m;
//        day = d;
//    }
//
//    friend void display(const Date& d, const Time& t);
//};
//
//class Time
//{
//private:
//    int hour, minute, second;
//
//public:
//    Time(int h, int m, int s)
//    {
//        hour = h;
//        minute = m;
//        second = s;
//    }
//
//    friend void display(const Date& d, const Time& t);
//};
//
//void display(const Date& d, const Time& t)
//{
//    cout << setfill('0')
//        << setw(4) << d.year << "-"
//        << setw(2) << d.month << "-"
//        << setw(2) << d.day << " "
//        << setw(2) << t.hour << ":"
//        << setw(2) << t.minute << ":"
//        << setw(2) << t.second << endl;
//}
//
//int main()
//{
//    int n;
//    cin >> n;
//
//    while (n--)
//    {
//        int y, mon, d;
//        int h, min, s;
//
//        cin >> y >> mon >> d;
//        cin >> h >> min >> s;
//
//        Date date(y, mon, d);
//        Time time(h, min, s);
//
//        display(date, time);
//    }
//
//    return 0;
//}