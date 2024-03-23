#include <iostream>
#include <vector>

using namespace std;

// Функция для вывода матрицы
void printMatrix(const vector<vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int element : row) {
            cout << element << " ";
        }
        cout << endl;
    }
}

/**
 * using Gauss elimination method
 * for searching matrix rank
 */
int rankOfMatrix(vector<vector<int>> matrix, int mod) {
    int rowCount = matrix.size();
    if (rowCount == 0) return 0;
    int colCount = matrix[0].size();

    int rank = 0;
    for (int col = 0; col < colCount; ++col) {
        int pivotRow = rank;
        while (pivotRow < rowCount && matrix[pivotRow][col] == 0) {
            ++pivotRow;
        }
        if (pivotRow == rowCount) continue;

        // rearrange the lines so that the current element is non-zero
        swap(matrix[rank], matrix[pivotRow]);

        // reset elements below the current element to zero
        for (int row = rank + 1; row < rowCount; ++row) {
            int factor = matrix[row][col] / matrix[rank][col];
            for (int i = col; i < colCount; ++i) {
                matrix[row][i] -= factor * matrix[rank][i];
                matrix[row][i] %= mod;
                // make sure the result remains non-negative
                if (matrix[row][i] < 0) matrix[row][i] += mod;
            }
        }
        ++rank;
    }
    return rank;
}

/**
 * using euclidian algorithm to get gcd/lcm of 2 uint's
 */
int gcd(int a, int b) { return b ? gcd(b, a % b) : a; }
int lcm(int a, int b) { return a / gcd(a, b) * b; }

int rankOfMatrix(int** orig_mat, int y_size, int x_size, int field_size) {
    int size = max(x_size, y_size);
    int rank = size;

    // Create a square matrix with zero filling for missing ones
    // elements
    vector<vector<int>> mat(size, vector<int>(size, 0));
    for (int i = 0; i < y_size; i++)
        for (int j = 0; j < x_size; j++) mat[i][j] = orig_mat[i][j];

    for (int row = 0; row < rank; row++) {
        if (mat[row][row] != 0) {
            for (int col = 0; col < size; col++) {
                if (col != row) {
                    if (mat[col][row] == 0) continue;
                    int lcm_value = lcm(mat[col][row], mat[row][row]);
                    int mult_rr = lcm_value / mat[row][row];
                    int mult_cr = lcm_value / mat[col][row];

                    for (int i = 0; i < rank; i++) {
                        long long mod_temp =
                            ((long long)(mult_cr * mat[col][i] -
                                         mult_rr * mat[row][i])) %
                            field_size;
                        if (mod_temp < 0) mod_temp += field_size;
                        mat[col][i] = mod_temp;
                    }
                }
            }
        } else {
            char reduce = 1;
            for (int i = row + 1; i < size; i++) {
                if (mat[i][row] != 0) {
                    swap(mat[row], mat[i]);
                    reduce = 0;
                    break;
                }
            }
            if (reduce) {
                rank--;
                for (int i = 0; i < size; i++) mat[i][row] = mat[i][rank];
                row--;
            }
        }
    }

    return rank;
}

int main() {
    // vector<vector<int>> matrix = {{1, -1, 2}, {2, -2, 4}, {1, -1, 2}};
    // {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    vector<vector<int>> matrix1 = {
        {1, 2, 0, 5}, {2, 4, 1, 0}, {-2, 4, 1, 0}, {1, 0, 2, 1}};
    cout << "Matrix:" << endl;
    printMatrix(matrix1);

    // Размеры матрицы
    const int rows = 4;
    const int cols = 4;

    // Выделение памяти для массива указателей на целочисленные массивы
    int** matrix2 = new int*[rows];

    // Выделение памяти для каждого целочисленного массива и инициализация
    // значений
    for (int i = 0; i < rows; ++i) {
        matrix2[i] = new int[cols];
        for (int j = 0; j < cols; ++j) {
            // Инициализация значений матрицы
            matrix1[i][j] = matrix1[i][j];
        }
    }

    cout << "Rank: " << rankOfMatrix(matrix1, 4) << endl;
    cout << "Rank: " << rankOfMatrix(matrix2, rows, cols, 4) << endl;

    return 0;
}
