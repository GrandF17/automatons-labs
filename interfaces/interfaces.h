#include "vector"

#ifndef INTERFACES
#define INTERFACES

using namespace std;
typedef unsigned int vTypeReg;
typedef long long int vTypeLin;

// ===== ========
// Shift Register
// ===== ========

struct shiftRegister {
    size_t stateLen;  // n
    vTypeReg state;
    vector<vTypeReg> psi, phi;
};

/**
 * eq classes:
 */
struct LFSREqClassMember {
    vTypeReg value;
    struct LFSREqClassMember *next;
};

struct LFSREqClass {
    vector<vTypeReg> fingerprint;
    struct LFSREqClassMember *members;
    struct LFSREqClass *next;
};

// ====== ==========
// Linear Automatons
// ====== ==========

struct linearAutomaton {
    size_t fieldSize;                      // q
    size_t inputLen, stateLen, outputLen;  // m,n,k
    vector<vector<vTypeLin>> A, B, C, D;
    vector<vTypeLin> state;
};

/**
 * eq classes:
 */
struct linEqClass {
    vector<vector<vTypeLin>> outputSeq, members;
};

#endif
