#include "vector"

#ifndef INTERFACES
#define INTERFACES

using namespace std;
typedef unsigned int uint;
typedef unsigned short ushort;

struct linearAutomaton {
    uint field_size;                        // q
    uint input_len, state_len, output_len;  // m,n,k
    vector<vector<uint>> A, B, C, D;
    vector<uint> state;
};

struct shiftRegister {
    uint state_len;  // n
    vector<char> psi, phi;
    uint state;
};

struct linEqClass {
    vector<vector<uint>> outputSeq, members;
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
#endif
