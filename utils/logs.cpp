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

/**
 * print only N elements at the beginning and N at the end:
 */
template <typename T>
void printGraphCompact(vector<vector<T>> adjacencyList) {
    size_t AMOUNT_TO_DISPLAY = 3;

    for (size_t i = 0; i < adjacencyList.size(); i++) {
        // printing 'start state':
        cout << "[" << i << "] --> ";

        // print all states where we can go from our 'start state':
        for (size_t j = 0; j < adjacencyList[i].size(); j++) {
            /**
             * print only part of vectors where goes state
             * (4 in the begininng & 4 in the end)
             */
            if (adjacencyList[i].size() > 10 &&
                (j > AMOUNT_TO_DISPLAY &&
                 j < adjacencyList[i].size() - (AMOUNT_TO_DISPLAY + 1)))
                continue;

            cout << "[" << adjacencyList[i][j] << "] ";
            if (j == AMOUNT_TO_DISPLAY) cout << "... ";
        }
        cout << endl;
    }
}

template <typename T>
vector<vector<T>> listToMatrix(vector<vector<T>> &adjacencyList) {
    T numNodes = adjacencyList.size();
    vector<vector<T>> matrix(numNodes, vector<T>(numNodes, 0));

    for (size_t i = 0; i < numNodes; ++i) {
        for (size_t j : adjacencyList[i]) {
            matrix[i][j] = 1;
        }
    }

    return matrix;
}

/**
 * print all vereces of graph:
 */
template <typename T>
void printGraph(vector<vector<T>> adjacencyList) {
    for (size_t i = 0; i < adjacencyList.size(); i++) {
        cout << "[" << i << "] --> ";
        for (size_t j = 0; j < adjacencyList[i].size(); j++)
            cout << "[" << adjacencyList[i][j] << "] ";

        cout << endl;
    }

    // vector<vector<T>> matrix = listToMatrix(adjacencyList);
    // ofstream outFile("matrix.txt");

    // if (outFile.is_open()) {
    //     for (size_t i = 0; i < matrix.size(); ++i) {
    //         for (size_t j = 0; j < matrix[i].size(); ++j) {
    //             outFile << matrix[i][j] << (j == matrix.size() - 1 ? "" : ",
    //             ");
    //         }
    //         outFile << endl;
    //     }
    //     outFile.close();
    //     cout << "Matrix has been written to file 'matrix.txt'" << endl;
    // } else {
    //     cerr << "Unable to open file" << endl;
    // }
}
#endif
