#include "../libraries/BigNumber.h"
#include "vector"

#ifndef INTERFACES
#define INTERFACES

using namespace std;
typedef unsigned int uint;

// ===== ========
// Shift Register
struct shiftRegister {
    uint state_len;  // n
    uint state;
    vector<uint> psi, phi;
};

struct LFSREqClassMember {
    uint value;
    struct LFSREqClassMember *next;
};

struct LFSREqClass {
    vector<uint> fingerprint;
    struct LFSREqClassMember *members;
    struct LFSREqClass *next;
};

// ====== ==========
// Linear Automatons
struct linEqClass {
    vector<vector<BigNumber>> outputSeq, members;
};

struct linearAutomaton {
    uint field_size;                        // q
    uint input_len, state_len, output_len;  // m,n,k
    vector<vector<BigNumber>> A, B, C, D;
    vector<BigNumber> state;
};

#endif
