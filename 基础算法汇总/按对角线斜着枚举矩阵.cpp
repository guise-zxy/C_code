#include <iostream>
#include <vector>
																				//从矩阵的左上角（或上方边缘）开始，向右下角（或右侧边缘）延伸
using namespace std;

void diagonalTraverse(const vector<vector<int>>& matrix) {
    int n = matrix.size();    // 行数
    if (n == 0) return;
    int m = matrix[0].size(); // 列数

    // 遍历所有对角线
    for (int d = 0; d < n + m - 1; d++) {													//左下到右上 
        // 确定当前对角线的起始行和列
        int row = min(d, n - 1); // 行从当前对角线的顶部开始
        int col = d - row;       // 列从当前对角线的左侧开始

        // 沿着对角线遍历
        while (row >= 0 && col < m) {
            cout << matrix[row][col] << " ";
            row--;          //向上 
            col++;			//向右 
        }
        cout << endl;
    }
}

int main() {
    // 示例矩阵
    vector<vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    // 沿着对角线遍历矩阵
    diagonalTraverse(matrix);

    return 0;
}


                                                                   //////////////////从右上到左下 
void diagonalTraverseTopRightToBottomLeft(const vector<vector<int>>& matrix) {
    int n = matrix.size();    // 行数
    if (n == 0) return;
    int m = matrix[0].size(); // 列数

    // 遍历所有对角线
    for (int d = 0; d < n + m - 1; d++) {
        // 确定当前对角线的起始行和列
        int row = min(d, m - 1); // 行从当前对角线的右侧开始
        int col = d - row;       // 列从当前对角线的顶部开始

        // 沿着对角线遍历（从右上到左下）
        while (row >= 0 && col < n) {
            cout << matrix[col][m - 1 - row] << " ";
            row--; // 向左移动
            col++; // 向下移动
        }
        cout << endl;
    }
}


