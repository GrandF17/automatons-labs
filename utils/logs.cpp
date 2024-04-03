#include <bitset>
#include <iostream>
#include <vector>

using namespace std;

#ifndef LOGGING
#define LOGGING

template <typename T>
void printVector(string title, vector<T> array) {
    cout << title << endl;
    for (size_t i = 0; i < array.size(); i++) cout << array[i] << " ";
    cout << endl;
}

template <typename T>
void printNumberAsBits(string title, T num) {
    cout << title << endl;
    bitset<sizeof(num)> bits(num);
    cout << num << " (" << bits << ")" << endl;
}

template <typename T>
void printNumberAsBits(T num) {
    bitset<sizeof(num)> bits(num);
    cout << num << " (" << bits << ")" << endl;
}

#endif
