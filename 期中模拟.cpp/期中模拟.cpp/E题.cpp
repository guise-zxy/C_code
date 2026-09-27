#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
using namespace std;

class CMatrix;   // 先声明矩阵类

class CVector
{
private:
    int* data;
    int n;

public:
    // 无参构造
    CVector()
    {
        n = 5;
        data = new int[n];

        for (int i = 0; i < n; i++)
            data[i] = i;
    }

    // 带参构造
    CVector(int n1, int a[])
    {
        n = n1;
        data = new int[n];

        for (int i = 0; i < n; i++)
            data[i] = a[i];
    }

    // 拷贝构造：深拷贝
    CVector(const CVector& v)
    {
        n = v.n;
        data = new int[n];

        for (int i = 0; i < n; i++)
            data[i] = v.data[i];
    }

    void print()
    {
        for (int i = 0; i < n; i++)
        {
            if (i != 0)
                cout << " ";
            cout << data[i];
        }
        cout << endl;
    }

    ~CVector()
    {
        delete[] data;
    }

    // CMatrix 是 CVector 的友元类
    friend class CMatrix;
};

class CMatrix
{
private:
    int** data;
    int n;

public:
    // 构造函数
    CMatrix(int n1, int** a)
    {
        n = n1;

        data = new int* [n];
        for (int i = 0; i < n; i++)
        {
            data[i] = new int[n];
            for (int j = 0; j < n; j++)
            {
                data[i][j] = a[i][j];
            }
        }
    }

    // 判断矩阵和向量是否可以相乘
    bool canMulti(const CVector& v1)
    {
        return n == v1.n;
    }

    // 矩阵 × 向量
    CVector multi(const CVector& v1)
    {
        int* result = new int[n];

        for (int i = 0; i < n; i++)
        {
            result[i] = 0;

            for (int j = 0; j < n; j++)
            {
                result[i] += data[i][j] * v1.data[j];
            }
        }

        CVector ans(n, result);

        delete[] result;

        return ans;
    }

    ~CMatrix()
    {
        for (int i = 0; i < n; i++)
            delete[] data[i];

        delete[] data;
    }
};

int main()
{
    int t;
    cin >> t;

    while (t--)
    {
        int n;
        cin >> n;

        int** matrixData = new int* [n];

        for (int i = 0; i < n; i++)
        {
            matrixData[i] = new int[n];

            for (int j = 0; j < n; j++)
            {
                cin >> matrixData[i][j];
            }
        }

        CMatrix matrix(n, matrixData);

        for (int i = 0; i < n; i++)
            delete[] matrixData[i];

        delete[] matrixData;

        int m;
        cin >> m;

        int* vectorData = new int[m];

        for (int i = 0; i < m; i++)
            cin >> vectorData[i];

        CVector v(m, vectorData);

        delete[] vectorData;

        if (!matrix.canMulti(v))
        {
            cout << "error" << endl;
        }
        else
        {
            matrix.multi(v).print();
        }
    }

    return 0;
}