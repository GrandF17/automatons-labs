#include <iostream>
#include <stdexcept>
#include <vector>

#include "BigNumber.h"  // Подключаем заголовочный файл BigNumber.h

using namespace std;

class MatrixMath {
   public:
    // ========
    // addition
    static vector<vector<BigNumber>> add(vector<vector<BigNumber>>& matrix1,
                                         vector<vector<BigNumber>>& matrix2) {
        if (matrix1.size() != matrix2.size() ||
            matrix1[0].size() != matrix2[0].size()) {
            throw invalid_argument(
                "Matrices must be of the same size for addition.");
        }

        vector<vector<BigNumber>> result(matrix1.size(),
                                         vector<BigNumber>(matrix1[0].size()));

        for (size_t i = 0; i < matrix1.size(); ++i) {
            for (size_t j = 0; j < matrix1[0].size(); ++j) {
                result[i][j] = matrix1[i][j].add(matrix2[i][j]);
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
    static vector<BigNumber> add(const vector<BigNumber>& myVector1,
                                 const vector<BigNumber>& myVector2) {
        vector<vector<BigNumber>> matrix1, matrix2;
        matrix1.push_back(myVector1);
        matrix2.push_back(myVector2);
        return add(matrix1, matrix2)[0];
    }

    // ==============
    // multiplication
    static vector<vector<BigNumber>> mul(vector<vector<BigNumber>>& matrix1,
                                         vector<vector<BigNumber>>& matrix2) {
        if (matrix1[0].size() != matrix2.size()) {
            throw invalid_argument(
                "Number of columns of first matrix must equal number of rows "
                "of second matrix for multiplication.");
        }

        vector<vector<BigNumber>> result(matrix1.size(),
                                         vector<BigNumber>(matrix2[0].size()));

        for (size_t i = 0; i < matrix1.size(); ++i) {
            for (size_t j = 0; j < matrix2[0].size(); ++j) {
                BigNumber sum("0");
                for (size_t k = 0; k < matrix1[0].size(); ++k) {
                    sum = sum.add(matrix1[i][k].mul(matrix2[k][j]));
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
    static vector<BigNumber> mul(vector<BigNumber>& myVector,
                                 vector<vector<BigNumber>>& matrix2) {
        vector<vector<BigNumber>> matrix1;
        matrix1.push_back(myVector);
        return mul(matrix1, matrix2)[0];
    }

    // =======
    // modulus
    static vector<vector<BigNumber>> mod(
        const vector<vector<BigNumber>>& matrix, const BigNumber& modulus) {
        vector<vector<BigNumber>> result(matrix);
        for (size_t i = 0; i < result.size(); ++i) {
            for (size_t j = 0; j < result[i].size(); ++j) {
                result[i][j] = result[i][j].mod(modulus);
            }
        }
        return result;
    }

    /**
     *  @overload of mod for vector
     */
    static vector<BigNumber> mod(const vector<BigNumber>& myVector,
                                 const BigNumber& modulus) {
        return mod(vector<vector<BigNumber>>{myVector}, modulus)[0];
    }

    // =====
    // print
    static void print(const vector<vector<BigNumber>>& matrix) {
        for (const auto& row : matrix) {
            for (const BigNumber& val : row) {
                cout << val << " ";
            }
            cout << endl;
        }
    }

    /**
     *  @overload of print for vector
     */
    static void print(const vector<BigNumber>& myVector) {
        print(vector<vector<BigNumber>>{myVector});
    }
};
