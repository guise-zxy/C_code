//#define _CRT_SECURE_NO_WARNINGS
//
//#include <iostream>
//using namespace std;
//
//class CVector
//{
//private:
//    int* data;   // 存储向量数据
//    int n;       // 向量维数
//
//public:
//    // 无参构造函数
//    CVector()
//    {
//        n = 5;
//        data = new int[n];
//
//        for (int i = 0; i < n; i++)
//        {
//            data[i] = i;
//        }
//    }
//
//    // 带参构造函数
//    CVector(int n1, int a[])
//    {
//        n = n1;
//        data = new int[n];
//
//        for (int i = 0; i < n; i++)
//        {
//            data[i] = a[i];
//        }
//    }
//
//    // 输出函数
//    void print()
//    {
//        for (int i = 0; i < n; i++)
//        {
//            if (i != 0)
//                cout << " ";
//            cout << data[i];
//        }
//        cout << endl;
//    }
//
//    // 析构函数
//    ~CVector()
//    {
//        delete[] data;
//    }
//};
//
//int main()
//{
//    int n;
//    cin >> n;
//
//    int* a = new int[n];
//
//    for (int i = 0; i < n; i++)
//    {
//        cin >> a[i];
//    }
//
//    CVector v1;
//    CVector v2(n, a);
//
//    v1.print();
//    v2.print();
//
//    delete[] a;
//
//    return 0;
//}