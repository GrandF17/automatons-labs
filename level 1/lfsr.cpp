#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../libraries/Matrix.h"
#include "../utils/logs.cpp"

using namespace std;

#ifndef LAB_ONE_LFSR
#define LAB_ONE_LFSR

struct shiftRegister initLFSR(ifstream& file) {
    struct shiftRegister lfsr;

    file >> lfsr.stateLen;
    file.ignore(2);

    size_t funcLen = pow(2, lfsr.stateLen + 1);
    lfsr.phi = vector<vTypeReg>(funcLen, 0);
    lfsr.psi = vector<vTypeReg>(funcLen, 0);

    for (vTypeReg i = 0; i < funcLen; i++) {
        file >> lfsr.phi[i];
        file.get();
    }
    file.get();

    for (vTypeReg i = 0; i < funcLen; i++) {
        file >> lfsr.psi[i];
        file.get();
    }

    lfsr.state = 0;

    return lfsr;
}

/**
 * linear feedback shift register implementation
 */
vTypeReg LFSRStep(struct shiftRegister* lfsr, vTypeReg input) {
    vTypeReg newBit = lfsr->phi[lfsr->state * 2 + input];
    vTypeReg output = lfsr->psi[lfsr->state * 2 + newBit];
    /**
     * << 1 - left shift (or multiplication by 2)
     * | - OR operation (1 | 0 = 1, 1 | 1 = 1, 0 | 0 = 0)
     * 0x1 << lfsr->stateLen meance that we mul 0x1 4 times by 2 --> 0x1111
     *
     * Example:
     * 0x0101 --> [shift by 1] --> 0x01010 -> [OR 0x00001] --> 0x01011 -->
     * --> [cuts elder bit using bitmask: (0x1 << 4 - 1) ~ (0x10000 - 1) ~
     * 0x1111] --> 0x1011
     */
    lfsr->state = ((lfsr->state << 1) | newBit) & ((0x1 << lfsr->stateLen) - 1);

    return output;
}

void LFSROperation(struct shiftRegister* lfsr) {
    cout << "\nEnter initial state (" << lfsr->stateLen
         << " numbers [0, 1]):" << endl;

    vTypeReg bit;  // bit entered by user in console
    for (int i = 0; i < lfsr->stateLen; i++) {
        cin >> bit;
        lfsr->state = (lfsr->state << 1) | bit;
    }

    while (true) {
        cout << "\n\nEnter new input (0/1): ";
        cin >> bit;

        printNumberAsBits("Output: ", LFSRStep(lfsr, bit));
        printNumberAsBits("New state: ", lfsr->state);
    }
}

#endif
