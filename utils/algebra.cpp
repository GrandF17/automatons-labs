#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <bitset>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../level 1/lfsr.cpp"
#include "../level 1/linear.cpp"
#include "../libraries/Matrix.h"

using namespace std;

#ifndef LAB_TWO_LIN
#define ALGEBRA

/**
 * serching a number C such that A * C = 1 (mod q)
 */
vTypeLin modInverse(vTypeLin A, vTypeLin mod) {
    int m0 = mod, t, q;
    int x0 = 0, x1 = 1;

    if (mod == 1) return 0;
    while (A > 1) {
        // q - quotient
        q = A / mod;
        t = mod;
        // m - remainder
        mod = A % mod, A = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;
    return x1;
}

/**
 * serching a number C such that A * C = B (mod q)
 */
vTypeLin findMultiplier(vTypeLin A, vTypeLin B, vTypeLin q) {
    int A_inverse = modInverse(A, q);
    int C = (A_inverse * B) % q;

    return C;
}

template <typename T>
vector<T> incrementVector(vector<T> state, T mod) {
    // increment junior rank by mod
    int juniorRank = state.size() - 1;
    state[juniorRank]++;

    // we move 1 up the ranks while there is overflow
    for (int i = juniorRank; i >= 0; i--) {
        if (state[i] >= (T)mod) {
            state[i] %= (T)mod;
            if (i - 1 >= 0) state[i - 1]++;
        }
    }

    return state;
}

#endif
