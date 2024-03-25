#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../libraries/BigNumber.h"
#include "../libraries/Matrix.h"

using namespace std;

#ifndef LAB_ONE
#define LAB_ONE

vector<vector<BigNumber>> readMatrix(int x, int y, ifstream& file) {
    vector<vector<BigNumber>> matrix(x, vector<BigNumber>(y));
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            string numberStr;
            file >> numberStr;  // Read the number as a string

            // Create a BigNumber object from the string and place it in the
            // matrix
            matrix[i][j] = numberStr;

            file.get();  // Skip the separator
        }
    }
    return matrix;
}

uint getRegByte(int reg, int size, int index) {
    return ((reg >> (size - index - 1)) & 0x01);
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

    lin.state = vector<BigNumber>(lin.state_len, BigNumber(0));

    return lin;
}

struct shiftRegister initLFSR(ifstream& file) {
    struct shiftRegister lfsr;

    file >> lfsr.state_len;
    file.ignore(2);

    uint func_len = pow(2, lfsr.state_len + 1);
    lfsr.phi = vector<uint>(func_len, 0);
    lfsr.psi = vector<uint>(func_len, 0);

    for (uint i = 0; i < func_len; i++) {
        file >> lfsr.phi[i];
        file.get();
    }
    file.get();

    for (uint i = 0; i < func_len; i++) {
        file >> lfsr.psi[i];
        file.get();
    }

    lfsr.state = 0;

    return lfsr;
}

/**
 * linear automaton implementation
 */
vector<BigNumber> linStep(struct linearAutomaton* lin,
                          vector<BigNumber> input) {
    vector<BigNumber> new_state =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->A),
                                        MatrixMath::mul(input, lin->B)),
                        BigNumber(lin->field_size));
    vector<BigNumber> output =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->C),
                                        MatrixMath::mul(input, lin->D)),
                        BigNumber(lin->field_size));

    // set new state in linear automaton
    lin->state = new_state;

    return output;
}

void linOperation(struct linearAutomaton* lin) {
    cout << "\nEnter initial state (" << lin->state_len << " numbers [0;"
         << (lin->field_size - 1) << "]):\n";
    for (uint i = 0; i < lin->state_len; i++) {
        string linStateStr;
        cin >> linStateStr;
        lin->state[i] = BigNumber(linStateStr);
    }

    while (true) {
        vector<BigNumber> input(lin->input_len);
        cout << "\n\nEnter new input (" << lin->input_len << " numbers [0;"
             << (lin->field_size - 1) << "]):\n";
        for (uint i = 0; i < lin->input_len; i++) {
            string outputStr;
            cin >> outputStr;
            input[i] = BigNumber(outputStr);
        }

        vector<BigNumber> output = linStep(lin, input);

        cout << "Output:\n";
        for (uint i = 0; i < lin->output_len; i++) cout << output[i] << " ";
        cout << "\nNew state:\n";
        for (uint i = 0; i < lin->state_len; i++) cout << lin->state[i] << " ";
        output.clear();
    }
}

/**
 * linear feedback shift register implementation
 */
uint LFSRStep(struct shiftRegister* lfsr, uint input) {
    uint new_bit = lfsr->phi[lfsr->state * 2 + input];
    uint output = lfsr->psi[lfsr->state * 2 + new_bit];
    /**
     * << 1 - left shift (or multiplication by 2)
     * | - OR operation (1 | 0 = 1, 1 | 1 = 1, 0 | 0 = 0)
     * 0x1 << lfsr->state_len meance that we mul 0x1 4 times by 2 --> 0x1111
     *
     * Example:
     * 0x0101 --> [shift by 1] --> 0x01010 -> [OR 0x00001] --> 0x01011 -->
     * --> [cuts elder bit using bitmask: (0x1 << 4 - 1) ~ (0x10000 - 1) ~
     * 0x1111] --> 0x1011
     */
    lfsr->state =
        ((lfsr->state << 1) | new_bit) & ((0x1 << lfsr->state_len) - 1);

    return output;
}

void LFSROperation(struct shiftRegister* lfsr) {
    cout << "\nEnter initial state (" << lfsr->state_len
         << " numbers [0;1]):" << endl;

    uint bit;  // bit entered by user in console
    for (int i = 0; i < lfsr->state_len; i++) {
        cin >> bit;
        lfsr->state = (lfsr->state << 1) | bit;
    }

    while (true) {
        cout << "\n\nEnter new input (0/1): ";
        cin >> bit;

        uint output = LFSRStep(lfsr, bit);
        cout << "Output: " << output;
        cout << "\nNew state: ";
        for (int i = 0; i < lfsr->state_len; i++)
            cout << getRegByte(lfsr->state, lfsr->state_len, i) << " ";
    }
}

#endif
