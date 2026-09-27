#include <iostream>
using namespace std;

class CVector
{
private:
    int* data;
    int n;

public:
    // 无参构造函数
    CVector()
    {
        n = 5;
        data = new int[n];

        for (int i = 0; i < n; i++)
            data[i] = i;
    }

    // 带参构造函数
    CVector(int n1, int a[])
    {
        n = n1;
        data = new int[n];

        for (int i = 0; i < n; i++)
            data[i] = a[i];
    }

    // 拷贝构造函数：深拷贝
    CVector(const CVector& v)
    {
        n = v.n;
        data = new int[n];

        for (int i = 0; i < n; i++)
            data[i] = v.data[i];
    }

    // 输出函数
    void print() const
    {
        for (int i = 0; i < n; i++)
        {
            if (i != 0)
                cout << " ";
            cout << data[i];
        }
        cout << endl;
    }

    // 友元函数声明
    friend CVector add(const CVector v1, const CVector v2);

    // 析构函数
    ~CVector()
    {
        delete[] data;
    }
};

// 友元函数定义
CVector add(const CVector v1, const CVector v2)
{
    int* sum = new int[v1.n];

    for (int i = 0; i < v1.n; i++)
        sum[i] = v1.data[i] + v2.data[i];

    CVector result(v1.n, sum);

    delete[] sum;

    return result;
}

int main()
{
    int t;
    cin >> t;

    while (t--)
    {
        int n;
        cin >> n;

        int* a = new int[n];
        int* b = new int[n];

        for (int i = 0; i < n; i++)
            cin >> a[i];

        for (int i = 0; i < n; i++)
            cin >> b[i];

        CVector v1(n, a);
        CVector v2(n, b);

        v1.print();
        v2.print();
        add(v1, v2).print();

        delete[] a;
        delete[] b;
    }

    return 0;
}