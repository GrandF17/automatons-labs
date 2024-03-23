#include "./lab-two.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../level 1/lab-one.cpp"
#include "../libraries/BigNumber.h"
#include "../libraries/Matrix.h"

using namespace std;

#ifndef LAB_TWO
#define LAB_TWO

bool nextVector(vector<uint>& vector, uint max) {
    int len = vector.size();
    for (int i = len - 1; i >= 0; i--) {
        if (vector[i] != max - 1) {
            vector[i]++;
            for (int j = i + 1; j < len; j++) vector[j] = 0;
            return true;
        }
    }
    return false;
}

bool addToClasses(LFSREqClass* classes, vector<int> fingerprint, int state) {
    bool result = false;
    LFSREqClass* currentClass = classes;

    while (currentClass) {
        LFSREqClassMember* member = currentClass->members;
        if (currentClass->fingerprint[0] == fingerprint[0] &&
            currentClass->fingerprint[1] == fingerprint[1]) {
            LFSREqClassMember* new_member = new LFSREqClassMember;
            new_member->value = state;
            new_member->next = currentClass->members;
            currentClass->members = new_member;
            result = true;
            break;
        }
        currentClass = currentClass->next;
    }
    return result;
}

void appendClass(LFSREqClass** classes, int state, vector<int> fingerprint) {
    LFSREqClassMember* members = new LFSREqClassMember;
    members->value = state;

    LFSREqClass* newClass = new LFSREqClass;
    newClass->members = members;
    newClass->fingerprint[0] = fingerprint[0];
    newClass->fingerprint[1] = fingerprint[1];
    newClass->next = *classes;
    *classes = newClass;
}

void printClasses(LFSREqClass* classes, int state_len) {
    LFSREqClass* currentClass = classes;
    cout << "-----------" << endl;
    while (currentClass) {
        cout << "CLASS " << currentClass->fingerprint[0] << " "
             << currentClass->fingerprint[1] << ": ";
        LFSREqClassMember* member = currentClass->members;
        while (member) {
            for (int i = 0; i < state_len; i++)
                cout << (((member->value) >> (state_len - i - 1)) & 0x01);
            cout << "|" << member->value << " ";
            member = member->next;
        }
        currentClass = currentClass->next;
        cout << endl;
    }
    cout << "-----------" << endl;
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

uint getClass(LFSREqClass* currentClass, uint state, uint classIndex) {
    /**
     * if members of this class do CONTAIN the required state -->
     * return classIndex
     */
    if (scanMembers(currentClass->members, state)) return classIndex;
    /**
     * if members of this class do NOT contain the required state BUT next is
     * defined --> go further
     */
    if (currentClass->next)
        return getClass(currentClass->next, state, ++classIndex);
    /**
     * if class not found AND next is undefined --> [-1]
     */
    return -1;
}

// ============================================
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

// ============================================
// =========== do smth ===========
// Функция сдвига регистра на один такт
void LFSRStep(shiftRegister* lfsr) {
    unsigned int feedback = 0;
    for (int i = 0; i < lfsr->state_len; ++i)
        feedback ^= (lfsr->state & (1 << i)) ? lfsr->psi[i] : 0;

    lfsr->state = (lfsr->state >> 1) | (feedback << (lfsr->state_len - 1));
}

// Функция для проверки эквивалентности двух состояний LFSR
bool statesEquivalent(const shiftRegister& lfsr1, const shiftRegister& lfsr2) {
    return (lfsr1.psi == lfsr2.psi && lfsr1.phi == lfsr2.phi &&
            lfsr1.state_len == lfsr2.state_len);
}

// Функция для печати классов эквивалентности
void printClasses(const vector<vector<unsigned int>>& classes) {
    for (size_t i = 0; i < classes.size(); ++i) {
        cout << "Class " << i + 1 << ": ";
        for (auto state : classes[i]) {
            cout << state << " ";
        }
        cout << endl;
    }
}

// Функция для печати классов эквивалентности
void printMembers(LFSREqClassMember member) {
    cout << member.value << " ";
    if (!member.next) {
        cout << endl;
        return;
    }
    printMembers(*member.next);
}

// Функция для печати классов эквивалентности
void printClasses(struct LFSREqClass classes) {
    cout << "Class: ";
    printMembers(*classes.members);
    if (!classes.next) return;
    printClasses(*classes.next);
}

void findLFSREqClasses(shiftRegister* lfsr) {}

/**
 * @param lin - linear automaton as Object containing 'state'
 * and matrices: A, B, C, D
 * @param size - power of C in diagnostic matrix
 * @returns diagnostic matrix: [C, C*A, C*A^2,  ... , C*A^{size - 1}]
 */
vector<vector<uint>> diagnosticMatrix(struct linearAutomaton* lin, uint size) {
    /**
     * using lib Matrix for mul of C and A
     */
    vector<vector<vector<uint>>> diagnosticMatrix;
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
        vector<vector<uint>> aMatrixPowered(lin->A.size(),
                                            vector<uint>(lin->A[0].size(), 0));
        for (int i = 0; i < aMatrixPowered.size(); ++i)
            aMatrixPowered[i][i] = 1;

        /**
         * raising A to the i-th power
         */
        for (uint aDegree = 0; aDegree < i; aDegree++)
            aMatrixPowered = MatrixMath::mod(
                MatrixMath::mul(aMatrixPowered, lin->A), lin->field_size);

        cout << "A^" << i << ":" << endl;
        MatrixMath::print(aMatrixPowered);
        cout << endl;

        cout << "A^" << i << "*C:" << endl;
        MatrixMath::print(MatrixMath::mul(aMatrixPowered, lin->C));
        cout << endl;

        cout << "A^" << i << "*C mod " << lin->field_size << ":" << endl;
        MatrixMath::print(MatrixMath::mod(
            MatrixMath::mul(aMatrixPowered, lin->C), lin->field_size));
        cout << endl;

        /**
         * C * A^i mod fieldSize
         */
        diagnosticMatrix.push_back(MatrixMath::mod(
            MatrixMath::mul(aMatrixPowered, lin->C), lin->field_size));
    }

    /**
     * conversion from 3D to 2D matrix placed horizontally
     */
    vector<vector<uint>> twoDimentionMatrix;
    for (uint j = 0; j < diagnosticMatrix[0].size(); j++) {
        vector<uint> row;
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
uint rankOfMatrix(vector<vector<uint>> matrix, uint mod) {
    int rowCount = matrix.size();
    if (rowCount == 0) return 0;
    int colCount = matrix[0].size();

    int rank = 0;  // can't be less than zero
    for (int col = 0; col < colCount; col++) {
        int pivotRow = rank;
        while (pivotRow < rowCount && matrix[pivotRow][col] == 0) ++pivotRow;
        if (pivotRow == rowCount) continue;

        // rearrange the lines so that the current element is non-zero
        swap(matrix[rank], matrix[pivotRow]);

        // reset elements below the current element to zero
        for (int row = rank + 1; row < rowCount; row++) {
            int factor = matrix[row][col] / matrix[rank][col];
            for (int i = col; i < colCount; i++) {
                matrix[row][i] -= (factor * matrix[rank][i]) % mod;
                // make sure the result remains non-negative
                if (matrix[row][i] < 0) matrix[row][i] += mod;
            }
        }
        rank++;
    }
    return rank;
}

void findLinEqClasses(linearAutomaton* lin) {
    uint delta = 0, prev_rank = 0, rank = 0;
    vector<vector<uint>> K;
    for (int i = 1; i <= lin->state_len; i++) {
        K = diagnosticMatrix(lin, i);
        rank = rankOfMatrix(K, lin->field_size);

        MatrixMath::print(K);
        cout << rank << " - rank" << endl;

        if (rank == prev_rank) {
            delta = i - 1;
            break;
        }
        prev_rank = rank;
        K.clear();
    }

    if (delta == 0) delta = lin->state_len;
    if (rank != lin->state_len) {
        K = diagnosticMatrix(lin, lin->state_len);
        rank = rankOfMatrix(K, lin->field_size);

        MatrixMath::print(K);
        cout << rank << " - rank" << endl;

        K.clear();
    }

    BigNumber mu = BigNumber(to_string(lin->field_size)).pow(to_string(rank));
    BigNumber compVar =
        BigNumber(to_string(lin->field_size)).pow(to_string(lin->state_len));

    cout << "mu=" << mu << ", delta=" << delta << endl;
    cout << "Automaton is " << (mu.eq(compVar) ? "" : "not ") << "minimal."
         << endl;
}

#endif
