#include <iostream>
#include <stdexcept>
#include <vector>

#ifndef MATRIX_LIB
#define MATRIX_LIB

using namespace std;

class MatrixMath {
   public:
    // ========
    // addition
    template <typename T>
    static vector<vector<T>> add(const vector<vector<T>>& matrix1,
                                 const vector<vector<T>>& matrix2) {
        if (matrix1.size() != matrix2.size() ||
            matrix1[0].size() != matrix2[0].size()) {
            throw invalid_argument(
                "Matrices must be of the same size for addition.");
        }

        vector<vector<T>> result(matrix1.size(), vector<T>(matrix1[0].size()));

        for (size_t i = 0; i < matrix1.size(); ++i) {
            for (size_t j = 0; j < matrix1[0].size(); ++j) {
                result[i][j] = matrix1[i][j] + matrix2[i][j];
            }
        }

        return result;
    }

    /**
     *  @overload of add for vectors
     * {1, 2, 3}
     * *
     * {1, 2, 3}
     */
    template <typename T>
    static vector<T> add(const vector<T>& matrix1, const vector<T>& matrix2) {
        return add(vector<vector<T>>{matrix1}, vector<vector<T>>{matrix2})[0];
    }

    // ==============
    // multiplication
    template <typename T>
    static vector<vector<T>> mul(const vector<vector<T>>& matrix1,
                                 const vector<vector<T>>& matrix2) {
        if (matrix1[0].size() != matrix2.size()) {
            throw invalid_argument(
                "Number of columns of first matrix must equal number of rows "
                "of second matrix for multiplication.");
        }

        vector<vector<T>> result(matrix1.size(), vector<T>(matrix2[0].size()));

        for (size_t i = 0; i < matrix1.size(); ++i) {
            for (size_t j = 0; j < matrix2[0].size(); ++j) {
                T sum = 0;
                for (size_t k = 0; k < matrix1[0].size(); ++k) {
                    sum += matrix1[i][k] * matrix2[k][j];
                }
                result[i][j] = sum;
            }
        }

        return result;
    }

    /**
     *  @overload of mul
     * {1, 2, 3}
     * *
     * {
     *  {1, 2}
     *  {3, 4}
     *  {5, 6}
     * }
     */
    template <typename T>
    static vector<T> mul(const vector<T>& matrix1,
                         const vector<vector<T>>& matrix2) {
        return mul(vector<vector<T>>{matrix1}, matrix2)[0];
    }

    // =======
    // modulus
    template <typename T>
    static vector<vector<T>> mod(const vector<vector<T>>& matrix,
                                 const T modulus) {
        vector<vector<T>> result(matrix);
        for (size_t i = 0; i < result.size(); ++i) {
            for (size_t j = 0; j < result[i].size(); ++j) {
                result[i][j] %= modulus;
            }
        }
        return result;
    }

    /**
     *  @overload of mod for vector
     */
    template <typename T>
    static vector<T> mod(const vector<T>& myVector, const T modulus) {
        return mod(vector<vector<T>>{myVector}, modulus)[0];
    }

    // =====
    // print
    template <typename T>
    static void print(const vector<vector<T>>& matrix) {
        for (const auto& row : matrix) {
            for (const T& val : row) {
                cout << val << " ";
            }
            cout << endl;
        }
    }

    /**
     *  @overload of print for vector
     */
    template <typename T>
    static void print(const vector<T>& myVector) {
        print(vector<vector<T>>{myVector});
    }
};

#endif
