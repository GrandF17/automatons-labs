#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmath>
#include <unordered_map>
#include <vector>

// by using 1 lab we can make steps in lin/lfsr automatons
#include "../level 1/lfsr.cpp"
#include "../level 1/linear.cpp"

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
    vector<T> currentState(lin->stateLen, 0);

    /**
     * go through all states
     */
    for (size_t i = 0; i < pow(lin->field_size, lin->stateLen) - 1; i++) {
        vector<T> currentInput(lin->inputLen, 0);
        vector<T> stateVector;

        /**
         * go through all inputs for current state
         */
        for (size_t j = 0; j < pow(lin->field_size, lin->inputLen); j++) {
            linStep(lin, currentInput);

            T asosiatedNumToVector = 0;
            for (size_t rank = 0; rank < lin->stateLen; rank++) {
                T degree = lin->stateLen - rank - 1;
                asosiatedNumToVector +=
                    lin->state[rank] * pow(lin->field_size, degree);
            }

            stateVector.push_back(asosiatedNumToVector);
            currentInput = incrementVector(currentInput, (T)lin->field_size);
        }

        adjacencyList.push_back(stateVector);
        currentState = incrementVector(currentState, (T)lin->field_size);
    }

    return adjacencyList;
}

/**
 * lfsr implementation
 */
template <typename T>
vector<vector<T>> initDirectedGraph(shiftRegister* lfsr) {
    vector<vector<T>> adjacencyList;
    T currentState = 0;

    /**
     * go through all states
     */
    for (size_t i = 0; i < pow(2, lfsr->stateLen); i++) {
        vector<T> stateVector;

        /**
         * go through all inputs (0/1) for current state
         * j - input (0/1)
         */
        for (size_t j = 0; j < 2; j++) {
            LFSRStep(lfsr, j);
            stateVector.push_back(lfsr->state);
        }

        adjacencyList.push_back(stateVector);
        currentState++;
    }

    return adjacencyList;
}

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
