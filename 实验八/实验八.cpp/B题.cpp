//#define _CRT_SECURE_NO_WARNINGS
//
//#include <iostream>
//using namespace std;
//
//class Complex
//{
//private:
//    double real; // Êµ²¿
//    double imag; // Ðé²¿
//
//public:
//    Complex()
//    {
//        real = 0;
//        imag = 0;
//    }
//
//    Complex(double r, double i)
//    {
//        real = r;
//        imag = i;
//    }
//
//    friend Complex addCom(const Complex& c1, const Complex& c2);
//    friend Complex minusCom(const Complex& c1, const Complex& c2);
//    friend void outCom(const Complex& c);
//};
//
//Complex addCom(const Complex& c1, const Complex& c2)
//{
//    return Complex(c1.real + c2.real, c1.imag + c2.imag);
//}
//
//Complex minusCom(const Complex& c1, const Complex& c2)
//{
//    return Complex(c1.real - c2.real, c1.imag - c2.imag);
//}
//
//void outCom(const Complex& c)
//{
//    cout << "(" << c.real << "," << c.imag << ")" << endl;
//}
//
//int main()
//{
//    double r, i;
//    cin >> r >> i;
//
//    Complex result(r, i);
//
//    int n;
//    cin >> n;
//
//    while (n--)
//    {
//        char op;
//        double r2, i2;
//        cin >> op >> r2 >> i2;
//
//        Complex temp(r2, i2);
//
//        if (op == '+')
//        {
//            result = addCom(result, temp);
//        }
//        else if (op == '-')
//        {
//            result = minusCom(result, temp);
//        }
//
//        outCom(result);
//    }
//
//    return 0;
//}