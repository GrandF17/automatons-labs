#include "./lab-two.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../level 1/lab-one.cpp"
#include "../libraries/BigNumber.h"
#include "../libraries/Matrix.h"

using namespace std;

#ifndef LAB_TWO
#define LAB_TWO

// === === ======= = =======
// add new classes / members
bool addToClasses(LFSREqClass* classes, const vector<uint>& fingerprint,
                  uint state) {
    if (!classes) return false;

    if (classes->fingerprint == fingerprint) {
        LFSREqClassMember* new_member = new LFSREqClassMember;
        new_member->value = state;
        new_member->next = classes->members;

        classes->members = new_member;
        return true;
    }

    if (!classes->next) return false;

    return addToClasses(classes->next, fingerprint, state);
}

void appendClass(LFSREqClass** classes, uint state, vector<uint> fingerprint) {
    LFSREqClassMember* members = new LFSREqClassMember;
    members->value = state;
    members->next = nullptr;

    LFSREqClass* newClass = new LFSREqClass;
    newClass->members = members;
    newClass->fingerprint = fingerprint;
    newClass->next = *classes;

    *classes = newClass;
}

/**
 * @returns amount of classes in singly linked list
 */
int getClassLen(LFSREqClass* currentClass) {
    if (!currentClass) return 0;
    return 1 + getClassLen(currentClass->next);
}

// ============================================
// ======= searching for specific class =======
bool scanMembers(LFSREqClassMember* member, uint state) {
    /**
     * if current member == state return true
     */
    if (member->value == state) return true;
    /**
     * if current member != state BUT next defined --> go further
     */
    if (member->next) return scanMembers(member->next, state);
    /**
     * if '!=' AND next is undefined --> false
     */
    return false;
}

/**
 * @returns class index and -1 if not found
 */
uint getClass(LFSREqClass* currentClass, uint state) {
    /**
     * if members of this class do CONTAIN the required state -->
     * return 0
     */
    if (scanMembers(currentClass->members, state)) return 0;
    /**
     * if members of this class do NOT contain the required state BUT next is
     * defined --> go further
     */
    if (currentClass->next) {
        int nextClassIndex = getClass(currentClass->next, state);
        // next class exists --> return 1 + 1 + ... + 0 (if we finally found)
        if (nextClassIndex != -1) return nextClassIndex + 1;
    }
    /**
     * if class not found AND next is undefined --> [-1]
     */
    return -1;
}

// ============ ===== ===== == === ============
// ============ frees space in mem ============
void clearMembers(LFSREqClassMember* member) {
    /**
     * before deleting this member we go to
     * the next until we found the last one
     */
    if (member->next) clearMembers(member->next);
    delete member;
}

void freeClasses(LFSREqClass* currentClass) {
    /**
     * before deleting this class we go to
     * the next until we found the last one
     */
    if (currentClass->next) freeClasses(currentClass->next);

    currentClass->fingerprint.clear();    // clear array of fingerprints
    clearMembers(currentClass->members);  // clear class members
    delete currentClass;                  // delete class field after all
}

// =====================================
// =========== print classes ===========
/**
 * printing members of current class
 */
void printMembers(LFSREqClassMember* member) {
    if (member == nullptr) return;

    // 4 * 4 = 16 --> 2^16 = max value of short
    bitset<sizeof(uint) * 4 /* bytes */> bits(member->value);
    cout << member->value << "(" << bits << ")"
         << " ";
    printMembers(member->next);
}

/**
 * printing current class
 */
void printClasses(LFSREqClass* classes) {
    if (classes == nullptr) return;

    cout << "Class " << classes->fingerprint[0] << classes->fingerprint[1]
         << ": ";
    printMembers(classes->members);
    cout << endl;

    printClasses(classes->next);
}

void findLFSREqClasses(shiftRegister* lfsr) {
    struct LFSREqClass* classes = nullptr;

    for (uint state = 0; state < pow(2, lfsr->state_len); state++) {
        vector<uint> fingerprint(2);

        lfsr->state = state;
        fingerprint[0] = LFSRStep(lfsr, 0);  // left result (as bin tree)

        lfsr->state = state;
        fingerprint[1] = LFSRStep(lfsr, 1);  // right result (as bin tree)

        if (!addToClasses(classes, fingerprint, state))
            appendClass(&classes, state, fingerprint);
    }

    // first print of classes
    cout << "First iteration: " << endl;
    printClasses(classes);

    uint iteration = 2;
    LFSREqClass *oldClasses = classes, *newClasses = nullptr;

    if (getClassLen(oldClasses) == 0) {
        cout << "mu=2^32, delta=1" << endl;
        return;
    }

    do {
        if (newClasses) {
            freeClasses(oldClasses);
            oldClasses = newClasses;
            newClasses = nullptr;
        }

        for (LFSREqClass* oldClass = oldClasses; oldClass;
             oldClass = oldClass->next) {
            LFSREqClass* localClass = nullptr;

            for (LFSREqClassMember* member = oldClass->members; member;
                 member = member->next) {
                uint state = member->value;
                vector<uint> fingerprint(2);

                lfsr->state = state;
                LFSRStep(lfsr, 0);
                fingerprint[0] = getClass(oldClasses, lfsr->state);

                lfsr->state = state;
                LFSRStep(lfsr, 1);
                fingerprint[1] = getClass(oldClasses, lfsr->state);

                char classFound = addToClasses(localClass, fingerprint, state);
                if (!classFound) appendClass(&localClass, state, fingerprint);
            }

            LFSREqClass* classPtr = localClass;
            while (classPtr->next) classPtr = classPtr->next;
            if (classPtr) classPtr->next = newClasses;
            newClasses = localClass;
        }

        cout << "Iteration " << iteration++ << ": " << endl;
        printClasses(newClasses);
    } while (getClassLen(oldClasses) != getClassLen(newClasses) &&
             getClassLen(newClasses) > 0);

    cout << "mu=" << getClassLen(newClasses) << ", delta=" << iteration - 2
         << endl;
    if (getClassLen(newClasses) == pow(2, lfsr->state_len))
        cout << "Automaton is minimal" << endl;
    else
        cout << "Automaton is not minimal" << endl;
    return;
}

/**
 * @param lin - linear automaton as Object containing 'state'
 * and matrices: A, B, C, D
 * @param size - power of C in diagnostic matrix
 * @returns diagnostic matrix: [C, C*A, C*A^2,  ... , C*A^{size - 1}]
 */
vector<vector<BigNumber>> diagnosticMatrix(struct linearAutomaton* lin,
                                           uint size) {
    /**
     * using lib Matrix for mul of C and A
     */
    vector<vector<vector<BigNumber>>> diagnosticMatrix;
    for (uint i = 0; i < size; i++) {
        /**
         * if first iteration, where only C is placed in diagonstic mx
         */
        if (i == 0) {
            diagnosticMatrix.push_back(lin->C);
            continue;
        }

        /**
         * creating a matrix with 1 along the main diagonal
         */
        vector<vector<BigNumber>> aMatrixPowered(
            lin->A.size(), vector<BigNumber>(lin->A[0].size(), 0));
        for (int i = 0; i < aMatrixPowered.size(); ++i)
            aMatrixPowered[i][i] = 1;

        /**
         * raising A to the i-th power
         */
        for (uint aDegree = 0; aDegree < i; aDegree++)
            aMatrixPowered =
                MatrixMath::mod(MatrixMath::mul(aMatrixPowered, lin->A),
                                BigNumber(lin->field_size));

        /**
         * printing details of matrix operations
         */
        if (false) {
            cout << "A^" << i << ":" << endl;
            MatrixMath::print(aMatrixPowered);
            cout << endl;

            cout << "A^" << i << "*C:" << endl;
            MatrixMath::print(MatrixMath::mul(aMatrixPowered, lin->C));
            cout << endl;

            cout << "A^" << i << "*C mod " << lin->field_size << ":" << endl;
            MatrixMath::print(
                MatrixMath::mod(MatrixMath::mul(aMatrixPowered, lin->C),
                                BigNumber(lin->field_size)));
            cout << endl;
        }

        /**
         * C * A^i mod fieldSize
         */
        diagnosticMatrix.push_back(
            MatrixMath::mod(MatrixMath::mul(aMatrixPowered, lin->C),
                            BigNumber(lin->field_size)));
    }

    /**
     * conversion from 3D to 2D matrix placed horizontally
     */
    vector<vector<BigNumber>> twoDimentionMatrix;
    for (uint j = 0; j < diagnosticMatrix[0].size(); j++) {
        vector<BigNumber> row;
        for (uint i = 0; i < diagnosticMatrix.size(); i++)
            for (uint k = 0; k < diagnosticMatrix[0][0].size(); k++)
                row.push_back(diagnosticMatrix[i][j][k]);

        twoDimentionMatrix.push_back(row);
    }
    return twoDimentionMatrix;
}

/**
 * using Gauss-Jordan elimination method
 * for searching matrix rank
 */
uint rankOfMatrix(vector<vector<BigNumber>> matrix, BigNumber mod) {
    uint rowCount = matrix.size();
    if (rowCount == 0) return 0;
    uint colCount = matrix[0].size();
    uint rank = 0;  // rank can't be less than zero

    for (uint col = 0; col < colCount; col++) {
        uint pivotRow = rank;
        while (pivotRow < rowCount && matrix[pivotRow][col] == 0) ++pivotRow;
        if (pivotRow == rowCount) continue;

        // rearrange the lines so that the current element is non-zero
        swap(matrix[rank], matrix[pivotRow]);

        // reset elements below the current element to zero
        for (uint row = rank + 1; row < rowCount; row++) {
            BigNumber factor = matrix[row][col] / matrix[rank][col];
            if (false) {
                cout << matrix[row][col] << "/"
                     << " = " << matrix[rank][col] << factor << endl;
            }

            for (uint i = col; i < colCount; i++) {
                matrix[row][i] -= (factor * matrix[rank][i]) % mod;
                // make sure the result remains non-negative
                if (matrix[row][i] < BigNumber(0)) matrix[row][i] += mod;
            }
        }
        rank++;
    }

    if (false) {
        cout << "Matrix: " << endl;
        MatrixMath::print(matrix);
    }
    cout << endl;

    return rank;
}

void findLinEqClasses(linearAutomaton* lin) {
    uint delta = 0, prev_rank = 0, rank = 0;
    vector<vector<BigNumber>> K;

    for (uint i = 1; i <= lin->state_len; i++) {
        K = diagnosticMatrix(lin, i);
        rank = rankOfMatrix(K, BigNumber(lin->field_size));

        MatrixMath::print(K);
        K.clear();
        cout << rank << " - rank" << endl;

        if (rank == prev_rank) {
            delta = i - 1;
            break;
        }
        prev_rank = rank;
    }

    if (delta == 0) delta = lin->state_len;
    if (rank != lin->state_len) {
        K = diagnosticMatrix(lin, lin->state_len);
        rank = rankOfMatrix(K, lin->field_size);

        MatrixMath::print(K);
        K.clear();

        cout << rank << " - rank" << endl;
    }

    BigNumber mu = BigNumber(lin->field_size).pow(BigNumber(rank));
    BigNumber compVar =
        BigNumber(lin->field_size).pow(BigNumber(lin->state_len));

    cout << "mu=" << mu << ", delta=" << delta << endl;
    cout << "Automaton is " << (mu.eq(compVar) ? "" : "not ") << "minimal."
         << endl;
}

#endif
