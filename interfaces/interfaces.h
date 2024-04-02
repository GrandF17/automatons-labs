#include "../libraries/BigNumber.h"
#include "vector"

#ifndef INTERFACES
#define INTERFACES

using namespace std;
typedef unsigned int uint;
typedef unsigned long long int ullint;

// ===== ========
// Shift Register
// ===== ========

struct shiftRegister {
    using VertexType = uint;
    uint state_len;  // n
    uint state;
    vector<VertexType> psi, phi;
};

/**
 * eq classes:
 */
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
// ====== ==========

struct linearAutomaton {
    using VertexType = ullint;
    uint field_size;                        // q
    uint input_len, state_len, output_len;  // m,n,k
    vector<vector<VertexType>> A, B, C, D;
    vector<VertexType> state;
};

/**
 * eq classes:
 */
struct linEqClass {
    vector<vector<ullint>> outputSeq, members;
};

#endif
