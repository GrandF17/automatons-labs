#include <unistd.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../level 1/lab-one.cpp"

#ifndef LAB_TWO_INTERFACE
#define LAB_TWO_INTERFACE
extern int verbose;

char nextVector(short* vector, int len, short max);

bool addToClasses(struct LFSREqClass* classes, int* fingerprint, int state);

void appendClass(struct LFSREqClass** classes, int state, int* fingerprint);

void printClasses(struct LFSREqClass* classes, int state_len);

int classesLen(struct LFSREqClass* classes);

unsigned int getClass(struct LFSREqClass* classes, unsigned int state);

void freeClasses(struct LFSREqClass* _class);

void findEqClasses(struct shiftRegister* lfsr);

#endif
