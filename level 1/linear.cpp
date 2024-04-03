#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../libraries/Matrix.h"

using namespace std;

#ifndef LAB_ONE_LIN
#define LAB_ONE_LIN

vector<vector<ullint>> readMatrix(int x, int y, ifstream& file) {
    vector<vector<ullint>> matrix(x, vector<ullint>(y));
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            ullint numberStr;
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

    file >> lin.field_size;
    file.ignore(2);
    file >> lin.input_len >> lin.state_len >> lin.output_len;
    file.ignore(2);

    uint n = lin.state_len, m = lin.input_len, k = lin.output_len;

    lin.A = readMatrix(n, n, file);
    lin.B = readMatrix(m, n, file);
    lin.C = readMatrix(n, k, file);
    lin.D = readMatrix(m, k, file);

    lin.state = vector<ullint>(lin.state_len, 0);

    return lin;
}

/**
 * linear automaton implementation
 */
vector<ullint> linStep(struct linearAutomaton* lin, vector<ullint> input) {
    vector<ullint> new_state =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->A),
                                        MatrixMath::mul(input, lin->B)),
                        (ullint)lin->field_size);
    vector<ullint> output =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->C),
                                        MatrixMath::mul(input, lin->D)),
                        (ullint)lin->field_size);

    /**
     * setting new state in linear automaton
     */
    lin->state = new_state;
    return output;
}

void linOperation(struct linearAutomaton* lin) {
    cout << "\nEnter initial state (" << lin->state_len << " numbers [0;"
         << (lin->field_size - 1) << "]):\n";
    for (uint i = 0; i < lin->state_len; i++) {
        ullint linStateStr;
        cin >> linStateStr;
        lin->state[i] = linStateStr;
    }

    while (true) {
        vector<ullint> input(lin->input_len);
        cout << "\n\nEnter new input (" << lin->input_len << " numbers [0;"
             << (lin->field_size - 1) << "]):\n";
        for (uint i = 0; i < lin->input_len; i++) {
            ullint outputStr;
            cin >> outputStr;
            input[i] = outputStr;
        }

        vector<ullint> output = linStep(lin, input);

        cout << "Output:\n";
        for (uint i = 0; i < lin->output_len; i++) cout << output[i] << " ";
        cout << "\nNew state:\n";
        for (uint i = 0; i < lin->state_len; i++) cout << lin->state[i] << " ";
        output.clear();
    }
}

#endif
