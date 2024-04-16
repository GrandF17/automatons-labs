#include "../level 1/linear.cpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <bitset>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../level 1/lfsr.cpp"
#include "../libraries/Matrix.h"
#include "../utils/algebra.cpp"

using namespace std;

#ifndef LAB_TWO_LIN
#define LAB_TWO_LIN

/**
 * @param lin - linear automaton as Object containing 'state'
 * and matrices: A, B, C, D
 * @param size - power of C in diagnostic matrix
 * @returns diagnostic matrix: [C, C*A, C*A^2,  ... , C*A^{size - 1}]
 */
vector<vector<vTypeLin>> diagnosticMatrix(struct linearAutomaton* lin,
                                          size_t size) {
    /**
     * using lib Matrix for mul of C and A
     */
    vector<vector<vector<vTypeLin>>> diagnosticMatrix;
    for (size_t i = 0; i < size; i++) {
        /**
         * if first iteration, where only C is placed in diagonstic mx
         */
        if (i == 0) {
            diagnosticMatrix.push_back(lin->C);
            continue;
        }

        /**
         * creating a matrix with 1 along the main diagonal
         */
        vector<vector<vTypeLin>> aMatrixPowered(
            lin->A.size(), vector<vTypeLin>(lin->A[0].size(), 0));
        for (int i = 0; i < aMatrixPowered.size(); ++i)
            aMatrixPowered[i][i] = 1;

        /**
         * raising A to the i-th power
         */
        for (size_t aDegree = 0; aDegree < i; aDegree++)
            aMatrixPowered =
                MatrixMath::mod(MatrixMath::mul(aMatrixPowered, lin->A),
                                (vTypeLin)lin->fieldSize);

        /**
         * C * A^i mod fieldSize
         */
        diagnosticMatrix.push_back(MatrixMath::mod(
            MatrixMath::mul(aMatrixPowered, lin->C), (vTypeLin)lin->fieldSize));
    }

    /**
     * conversion from 3D to 2D matrix placed horizontally
     */
    vector<vector<vTypeLin>> twoDimentionMatrix;
    for (size_t j = 0; j < diagnosticMatrix[0].size(); j++) {
        vector<vTypeLin> row;
        for (size_t i = 0; i < diagnosticMatrix.size(); i++)
            for (size_t k = 0; k < diagnosticMatrix[0][0].size(); k++)
                row.push_back(diagnosticMatrix[i][j][k]);

        twoDimentionMatrix.push_back(row);
    }
    return twoDimentionMatrix;
}

/**
 * using Gauss-Jordan elimination method
 * for searching matrix rank
 */
size_t rankOfMatrix(vector<vector<vTypeLin>> matrix, vTypeLin mod) {
    size_t rowCount = matrix.size();
    if (rowCount == 0) return 0;
    size_t colCount = matrix[0].size();
    size_t rank = 0;  // rank can't be less than zero

    for (size_t col = 0; col < colCount; col++) {
        size_t pivotRow = rank;
        while (pivotRow < rowCount && matrix[pivotRow][col] == 0) ++pivotRow;
        if (pivotRow == rowCount) continue;

        // rearrange the lines so that the current element is non-zero
        swap(matrix[rank], matrix[pivotRow]);

        // reset elements below the current element to zero
        for (size_t row = rank + 1; row < rowCount; row++) {
            vTypeLin mulGF =
                findMultiplier(matrix[rank][col], matrix[row][col], mod);

            for (size_t i = col; i < colCount; i++) {
                matrix[row][i] -= (matrix[rank][i] * mulGF) % mod;
                // make sure the result remains non-negative
                if (matrix[row][i] < 0) matrix[row][i] += mod;
            }
        }
        rank++;
    }

    return rank;
}

void findLinEqClasses(linearAutomaton* lin) {
    size_t delta = 0, prevRank = 0, rank = 0;

    for (size_t i = 1; i <= lin->stateLen; i++) {
        vector<vector<vTypeLin>> K = diagnosticMatrix(lin, i);
        rank = rankOfMatrix(K, (vTypeLin)lin->fieldSize);

        MatrixMath::print(K);
        K.clear();
        cout << rank << " - rank" << endl;

        if (rank == prevRank) {
            delta = i - 1;
            break;
        }
        prevRank = rank;
    }

    if (delta == 0) delta = lin->stateLen;

    vTypeLin mu = (vTypeLin)pow((vTypeLin)lin->fieldSize, ((vTypeLin)rank));
    vTypeLin compVar =
        (vTypeLin)pow((vTypeLin)lin->fieldSize, (vTypeLin)lin->stateLen);

    cout << "Amount of equal state classes: " << mu << endl;
    cout << "Diff coefficient: " << delta << endl;
    cout << "Automaton is " << ((mu == compVar) ? "" : "not ") << "minimal."
         << endl;
}

#endif
