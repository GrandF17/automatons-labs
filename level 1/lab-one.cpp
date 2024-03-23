#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../libraries/Matrix.h"

using namespace std;

#ifndef LAB_ONE
#define LAB_ONE

vector<vector<ushort>> readMatrix(int x, int y, ifstream& file) {
    vector<vector<ushort>> matrix(x, vector<ushort>(y));
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            file >> matrix[i][j];
            file.get();  // skip separator
        }
    }
    file.get();  // skip separator
    return matrix;
}

char getRegByte(int reg, int size, int index) {
    return static_cast<char>((reg >> (size - index - 1)) & 0x01);
}

struct linearAutomaton initLinear(ifstream& file) {
    struct linearAutomaton lin;

    file >> lin.field_size;
    file.ignore(2);
    file >> lin.input_len >> lin.state_len >> lin.output_len;
    file.ignore(2);

    unsigned int n = lin.state_len;
    unsigned int m = lin.input_len;
    unsigned int k = lin.output_len;

    lin.A = readMatrix(n, n, file);
    lin.B = readMatrix(m, n, file);
    lin.C = readMatrix(n, k, file);
    lin.D = readMatrix(m, k, file);

    lin.state = vector<ushort>(lin.state_len, 0);

    return lin;
}

struct shiftRegister initLFSR(ifstream& file) {
    struct shiftRegister lfsr;

    file >> lfsr.state_len;
    file.ignore(2);

    int func_len = pow(2, lfsr.state_len + 1);
    lfsr.phi = vector<char>(func_len, 0);
    lfsr.psi = vector<char>(func_len, 0);

    for (int i = 0; i < func_len; i++) {
        file >> lfsr.phi[i];
        file.get();
    }
    file.get();

    for (int i = 0; i < func_len; i++) {
        file >> lfsr.psi[i];
        file.get();
    }

    lfsr.state = 0;

    return lfsr;
}

void printAutomatons(struct linearAutomaton lin, struct shiftRegister lfsr) {
    cout << "LINEAR\n\n  q=" << lin.field_size << endl;
    cout << "  m=" << lin.input_len << endl;
    cout << "  n=" << lin.state_len << endl;
    cout << "  k=" << lin.output_len << endl << endl;

    int n = lin.state_len;
    int m = lin.input_len;
    int k = lin.output_len;

    cout << "A:\n";
    MatrixMath::print(lin.A);
    cout << "B:\n";
    MatrixMath::print(lin.B);
    cout << "C:\n";
    MatrixMath::print(lin.C);
    cout << "D:\n";
    MatrixMath::print(lin.D);

    cout << "\n\nLFSR\n\nn=" << lfsr.state_len << endl << endl;
    for (int i = 0; i < pow(2, lfsr.state_len + 1); i++)
        cout << static_cast<int>(lfsr.psi[i]) << " ";
    cout << "\nphi:\n";
    for (int i = 0; i < pow(2, lfsr.state_len + 1); i++)
        cout << static_cast<int>(lfsr.phi[i]) << " ";
    cout << endl;
}

/**
 * linear automaton implementation
 */
vector<ushort> linStep(struct linearAutomaton* lin, vector<ushort> input) {
    vector<ushort> new_state =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->A),
                                        MatrixMath::mul(input, lin->B)),
                        lin->field_size);
    vector<ushort> output =
        MatrixMath::mod(MatrixMath::add(MatrixMath::mul(lin->state, lin->C),
                                        MatrixMath::mul(input, lin->D)),
                        lin->field_size);

    // set new state in linear automaton
    lin->state.clear();
    lin->state.resize(new_state.size());
    for (size_t i = 0; i < new_state.size(); ++i)
        lin->state[i] = static_cast<ushort>(new_state[i]);

    return output;
}

void linOperation(struct linearAutomaton* lin) {
    cout << "\nEnter initial state (" << lin->state_len << " numbers [0;"
         << (lin->field_size - 1) << "]):\n";
    for (int i = 0; i < lin->state_len; i++) cin >> lin->state[i];

    while (true) {
        vector<ushort> input(lin->input_len);
        cout << "\n\nEnter new input (" << lin->input_len << " numbers [0;"
             << (lin->field_size - 1) << "]):\n";
        for (int i = 0; i < lin->input_len; i++) cin >> input[i];

        vector<ushort> output = linStep(lin, input);

        cout << "Output:\n";
        for (int i = 0; i < lin->output_len; i++) cout << output[i] << " ";
        cout << "\nNew state:\n";
        for (int i = 0; i < lin->state_len; i++) cout << lin->state[i] << " ";
        output.clear();
    }
}

/**
 * linear feedback shift register implementation
 */
char LFSRStep(struct shiftRegister* lfsr, char input) {
    char new_bit = lfsr->phi[lfsr->state * 2 + input];
    char output = lfsr->psi[lfsr->state * 2 + new_bit];
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

    char bit;  // bit entered by user in console
    for (int i = 0; i < lfsr->state_len; i++) {
        cin >> bit;
        lfsr->state = (lfsr->state << 1) | bit;
    }

    while (true) {
        cout << "\n\nEnter new input (0/1): ";
        cin >> bit;

        char output = LFSRStep(lfsr, bit);
        cout << "Output: " << static_cast<int>(output);
        cout << "\nNew state: ";
        for (int i = 0; i < lfsr->state_len; i++)
            cout << static_cast<int>(
                        getRegByte(lfsr->state, lfsr->state_len, i))
                 << " ";
    }
}

#endif
