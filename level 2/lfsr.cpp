#include "../level 1/lfsr.cpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../level 1/linear.cpp"
#include "../libraries/Matrix.h"
#include "../utils/logs.cpp"

using namespace std;

#ifndef LAB_TWO_REG
#define LAB_TWO_REG

// === === ======= = =======
// add new classes / members
bool addToClasses(LFSREqClass* classes, const vector<vTypeReg>& fingerprint,
                  vTypeReg state) {
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

void appendClass(LFSREqClass** classes, vTypeReg state,
                 vector<vTypeReg> fingerprint) {
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
bool scanMembers(LFSREqClassMember* member, vTypeReg state) {
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
vTypeReg getClass(LFSREqClass* currentClass, vTypeReg state) {
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
    printNumberAsBits(member->value);
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

    for (vTypeReg state = 0; state < pow(2, lfsr->stateLen); state++) {
        vector<vTypeReg> fingerprint(2);

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

    vTypeReg iteration = 2;
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
                vTypeReg state = member->value;
                vector<vTypeReg> fingerprint(2);

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
    if (getClassLen(newClasses) == pow(2, lfsr->stateLen))
        cout << "Automaton is minimal" << endl;
    else
        cout << "Automaton is not minimal" << endl;
    return;
}

#endif
