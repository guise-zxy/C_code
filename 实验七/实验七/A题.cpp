//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <iomanip>
//#include <cmath>
//using namespace std;
//
//class Equation {
//private:
//    double a, b, c;
//
//public:
//    Equation();
//    Equation(double a, double b, double c);
//
//    void set(double a, double b, double c);
//    void getRoot();
//};
//
//// 无参构造
//Equation::Equation() {
//    this->a = 1.0;
//    this->b = 1.0;
//    this->c = 0;
//}
//
//// 有参构造
//Equation::Equation(double a, double b, double c) {
//    this->a = a;
//    this->b = b;
//    this->c = c;
//}
//
//// set方法
//void Equation::set(double a, double b, double c) {
//    this->a = a;
//    this->b = b;
//    this->c = c;
//}
//
//// 求根
//void Equation::getRoot() {
//    double delta = b * b - 4 * a * c;
//
//    if (delta == 0) {
//        double x = -b / (2 * a);
//        cout << "x1=x2=" << x << endl;
//    }
//    else if (delta > 0) {
//        double x1 = (-b + sqrt(delta)) / (2 * a);
//        double x2 = (-b - sqrt(delta)) / (2 * a);
//        cout << "x1=" << x1 << " x2=" << x2 << endl;
//    }
//    else {
//        double real = -b / (2 * a);
//        double imag = sqrt(-delta) / (2 * a);
//
//        cout << "x1=" << real << "+" << imag << "i "
//            << "x2=" << real << "-" << imag << "i" << endl;
//    }
//}
//
//int main() {
//    int t;
//    cin >> t;
//
//    cout << fixed << setprecision(2);
//
//    while (t--) {
//        double a, b, c;
//        cin >> a >> b >> c;
//
//        Equation e(a, b, c);
//        e.getRoot();
//    }
//
//    return 0;
//}