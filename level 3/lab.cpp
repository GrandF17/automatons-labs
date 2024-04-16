#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unordered_map>
#include <vector>

// by using 1 lab we can make steps in lin/lfsr automatons
#include "../level 1/lfsr.cpp"
#include "../level 1/linear.cpp"
#include "../level 2/linear.cpp"

// adiditonal modules/functions (utils)
#include "../interfaces/interfaces.h"
#include "../utils/algebra.cpp"
#include "../utils/graphAlgos.cpp"
#include "../utils/logs.cpp"

using namespace std;

#ifndef LAB_THREE
#define LAB_THREE

// ======== ===== ===
// creating graph ===
// ======== ===== ===

/**
 * linear implementation
 *
 * We got adjacency list:
 * [
 *      [[0, 0, 0, 0] -> [2, 3, 4, 5], more other vectors]
 *      ...
 * ]
 *
 * BUT these vectors are stored as digits!
 *
 * Example:
 * field size = 3 -->
 * --> [0, 1, 1, 0] ~ 1*3^2 + 1*3^1 + 0*3^0 = 12
 */
template <typename T>
vector<vector<T>> initDirectedGraph(linearAutomaton* lin) {
    vector<vector<T>> adjacencyList;

    /**
     * declare initial state
     * + set up zero vector in lin automaton struct
     */
    lin->state = vector<T>(lin->stateLen, 0);
    vector<T> state = lin->state;

    /**
     * go through all states
     */
    for (size_t i = 0; i < (T)pow(lin->fieldSize, lin->stateLen); i++) {
        vector<T> currentInput(lin->inputLen, 0);
        vector<T> allStates;

        /**
         * go through all inputs for current state
         *
         * we convert the data type to T because(T)pow returns a floating
         * point value and may have errors
         */
        for (size_t j = 0; j < (T)pow(lin->fieldSize, lin->inputLen); j++) {
            /**
             * cause state changes every step we need to set
             * it back on each input
             */
            lin->state = state;

            linStep(lin, currentInput);

            T associatedNumToVector = 0;
            for (size_t rank = 0; rank < lin->stateLen; rank++) {
                T degree = lin->stateLen - rank - 1;

                /**
                 * HOLY MOLY POW FROM THE OFICIAL C++
                 * LIB WORKS INCORRECT!!!
                 *
                 * sooooo... using own version for our cases...
                 */
                associatedNumToVector +=
                    lin->state[rank] * powEnchanced(lin->fieldSize, degree);
            }

            allStates.push_back(associatedNumToVector);
            currentInput = incrementVector(currentInput, (T)lin->fieldSize);
        }

        adjacencyList.push_back(allStates);
        state = incrementVector(state, (T)lin->fieldSize);
    }

    return adjacencyList;
}

/**
 * lfsr implementation
 */
template <typename T>
vector<vector<T>> initDirectedGraph(struct shiftRegister* lfsr) {
    vector<vector<T>> adjacencyList;

    /**
     * go through all states
     */
    for (size_t state = 0; state < (T)pow(2, lfsr->stateLen); state++) {
        vector<T> allStates;

        /**
         * go through all inputs (0/1) for current state
         * j - input (0/1)
         */
        for (size_t j = 0; j < 2; j++) {
            /**
             * cause state changes every step we need to set
             * it back on each input
             */
            lfsr->state = state;

            LFSRStep(lfsr, j);
            allStates.push_back(lfsr->state);
        }

        adjacencyList.push_back(allStates);
    }

    return adjacencyList;
}

bool linearCritereaCheck(linearAutomaton* lin) {
    vector<vector<vTypeLin>> matrix;
    vTypeLin previousRank = 0;
    size_t index = 0;

    cout << "================================" << endl;  // additional line skip
    while (true) {
        if (index == 0) {
            for (size_t j = 0; j < lin->B.size(); j++) {
                matrix.push_back(lin->B[j]);
            }
            previousRank = rankOfMatrix(matrix, (vTypeLin)lin->fieldSize);

            cout << "Matrix B: " << endl;
            MatrixMath::print(matrix);
            cout << "Rank: " << rankOfMatrix(matrix, (vTypeLin)lin->fieldSize)
                 << endl;

            index++;
            continue;
        }

        /**
         * creating a matrix with 1 along the main diagonal
         */
        vector<vector<vTypeLin>> aMatrixPowered(
            lin->A.size(), vector<vTypeLin>(lin->A[0].size(), 0));
        for (size_t i = 0; i < aMatrixPowered.size(); ++i)
            aMatrixPowered[i][i] = 1;

        /**
         * raising A to the i-th(T)power
         */
        for (size_t aDegree = 0; aDegree < index; aDegree++)
            aMatrixPowered =
                MatrixMath::mod(MatrixMath::mul(aMatrixPowered, lin->A),
                                (vTypeLin)lin->fieldSize);

        /**
         * B * A^i mod fieldSize
         */
        vector<vector<vTypeLin>> tmp = MatrixMath::mod(
            MatrixMath::mul(lin->B, aMatrixPowered), (vTypeLin)lin->fieldSize);
        for (size_t j = 0; j < tmp.size(); j++) {
            matrix.push_back(tmp[j]);
        }

        vTypeLin rank = rankOfMatrix(matrix, (vTypeLin)lin->fieldSize);
        cout << "Matrix (B ...  B * A^" << index << "): " << endl;
        MatrixMath::print(matrix);
        cout << "Rank: " << rank << endl;

        if (previousRank == rank) break;
        previousRank = rankOfMatrix(matrix, (vTypeLin)lin->fieldSize);

        index++;
    }

    cout << "Criteria says that automaton is "
         << (lin->stateLen == previousRank ? "" : "not ")
         << "strongly connected." << endl;
    cout << "================================" << endl;  // additional line skip
    return lin->stateLen == previousRank;
}

bool linearCritereaCheck(shiftRegister* lfsr) { return false; }

// ==== ============= ===
// main functionality ===
// ==== ============= ===

void printConclusion(bool isStronglyConnected, bool isConnected) {
    /**
     * if automaton is STRONGLY connected it is also connected
     */
    if (isStronglyConnected)
        cout << "Automaton is strongly connected." << endl;
    else if (isConnected)
        cout << "Automaton is connected." << endl;
    else if (!isConnected && !isStronglyConnected)
        cout << "Automaton is NOT connected." << endl;
}

/**
 * checking the graph for connectivity
 */
template <typename AutomatonType, typename vertexType>
void graphConnectivityCheck(AutomatonType* automaton) {
    vector<vector<vertexType>> adjacencyList =
        initDirectedGraph<vertexType>(automaton);

    if (false) printGraph(adjacencyList);

    /**
     * watch if criteria is met
     */
    linearCritereaCheck(automaton);

    /**
     * detecting if automaton is STRONGLY connected
     */
    bool isStronglyConnected = kosarajuAlgorithm(adjacencyList);
    if (isStronglyConnected) {
        printConclusion(isStronglyConnected, true);
        return;
    }

    /**
     * creating map [{0 -> status}, {1 -> status}, ...]
     * and fill it whith vertexStatus.WHITE
     */
    unordered_map<vertexType, vertexStatus> visited;
    for (vertexType vertex = 0; vertex < adjacencyList.size(); vertex++)
        visited[vertex] = vertexStatus::WHITE;

    /**
     * detecting if automaton is connected
     */
    vector<vector<vertexType>> undirectedGraph = dirToUndirGraph(adjacencyList);
    if (false) printGraph(undirectedGraph);

    vector<vertexType> visitResult =
        DFS(undirectedGraph, visited, (vertexType)0);
    bool isConnected = visitResult.size() == adjacencyList.size();

    printConclusion(isStronglyConnected, isConnected);
}

#endif
