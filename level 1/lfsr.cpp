#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/interfaces.h"

using namespace std;

#ifndef LAB_ONE_LFSR
#define LAB_ONE_LFSR

uint getRegByte(int reg, int size, int index) {
    return ((reg >> (size - index - 1)) & 0x01);
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
