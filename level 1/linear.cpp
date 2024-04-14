#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../libraries/Matrix.h"
#include "../utils/logs.cpp"

using namespace std;

#ifndef LAB_ONE_LIN
#define LAB_ONE_LIN

vector<vector<vTypeLin>> readMatrix(int x, int y, ifstream& file) {
    vector<vector<vTypeLin>> matrix(x, vector<vTypeLin>(y));
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            vTypeLin numberStr;
            file >> numberStr;  // Read the number as a string

            // Create a BigNumber object from the string and place it in the
            // matrix
            matrix[i][j] = numberStr;

            file.get();  // Skip the separator
        }
    }
    return matrix;
}

struct linearAutomaton initLinear(ifstream& file) {
    struct linearAutomaton lin;

    file >> lin.fieldSize;
    file.ignore(2);
    file >> lin.inputLen >> lin.stateLen >> lin.outputLen;
    file.ignore(2);

    size_t n = lin.stateLen, m = lin.inputLen, k = lin.outputLen;

    lin.A = readMatrix(n, n, file);
    lin.B = readMatrix(m, n, file);
    lin.C = readMatrix(n, k, file);
    lin.D = readMatrix(m, k, file);

    lin.state = vector<vTypeLin>(lin.stateLen, 0);

    return lin;
}

/**
 * linear automaton implementation
 */
vector<vTypeLin> linStep(struct linearAutomaton* lin, vector<vTypeLin> input) {
    vector<vTypeLin> new_state =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->A),
                                        MatrixMath::mul(input, lin->B)),
                        (vTypeLin)lin->fieldSize);
    vector<vTypeLin> output =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->C),
                                        MatrixMath::mul(input, lin->D)),
                        (vTypeLin)lin->fieldSize);

    /**
     * setting new state in linear automaton
     */
    lin->state = new_state;
    return output;
}

void linOperation(struct linearAutomaton* lin) {
    cout << "\nEnter initial state (" << lin->stateLen << " numbers [0;"
         << (lin->fieldSize - 1) << "]):\n";

    for (size_t i = 0; i < lin->stateLen; i++) {
        vTypeLin linStateStr;
        cin >> linStateStr;
        lin->state[i] = linStateStr;
    }

    while (true) {
        vector<vTypeLin> input(lin->inputLen);
        cout << "\n\nEnter new input (" << lin->inputLen << " numbers [0;"
             << (lin->fieldSize - 1) << "]):\n";

        for (size_t i = 0; i < lin->inputLen; i++) {
            vTypeLin outputStr;
            cin >> outputStr;
            input[i] = outputStr;
        }

        printVector("Output: ", linStep(lin, input));
        printVector("New State: ", lin->state);
    }
}

#endif
