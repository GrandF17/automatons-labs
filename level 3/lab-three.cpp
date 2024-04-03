#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmath>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../interfaces/interfaces.h"
#include "../level 1/lfsr.cpp"
#include "../level 1/linear.cpp"

using namespace std;

#ifndef LAB_THREE
#define LAB_THREE

template <typename T>
vector<T> incrementVector(vector<T> state, T mod) {
    // increment junior rank by mod
    int juniorRank = state.size() - 1;
    state[juniorRank]++;

    // we move 1 up the ranks while there is overflow
    for (int i = juniorRank; i >= 0; i--) {
        if (state[i] >= (T)mod) {
            state[i] %= (T)mod;
            if (i - 1 >= 0) state[i - 1]++;
        }
    }

    return state;
}

/**
 * print our linear graph:
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

/**
 * print our lfsr graph:
 */
template <typename T>
void printGraph(vector<vector<T>> adjacencyList) {
    for (size_t i = 0; i < adjacencyList.size(); i++) {
        cout << "[" << i << "] --> ";
        for (size_t j = 0; j < adjacencyList[i].size(); j++)
            cout << "[" << adjacencyList[i][j] << "] ";

        cout << endl;
    }
}

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
    vector<T> currentState(lin->state_len, 0);

    /**
     * go through all states
     */
    for (size_t i = 0; i < pow(lin->field_size, lin->state_len) - 1; i++) {
        vector<T> currentInput(lin->input_len, 0);
        vector<T> stateVector;

        /**
         * go through all inputs for current state
         */
        for (size_t j = 0; j < pow(lin->field_size, lin->input_len); j++) {
            linStep(lin, currentInput);

            T asosiatedNumToVector = 0;
            for (size_t rank = 0; rank < lin->state_len; rank++) {
                T degree = lin->state_len - rank - 1;
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
    for (size_t i = 0; i < pow(2, lfsr->state_len); i++) {
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

// ========== ===
// algorythms ===
// ========== ===

template <typename T>
vector<vector<T>> reverseGraph(vector<vector<T>> graph) {
    vector<vector<T>> reversedGraph(graph.size());

    for (size_t i = 0; i < graph.size(); ++i)
        for (const T& neighbor : graph[i]) reversedGraph[neighbor].push_back(i);

    return reversedGraph;
}

template <typename T>
vector<vector<T>> initUndirectedGraph(vector<vector<T>> directedGraph) {
    vector<vector<T>> reversedGraph = reverseGraph(directedGraph);

    for (size_t i = 0; i < directedGraph.size(); i++)
        directedGraph[i].insert(directedGraph[i].end(),
                                reversedGraph[i].begin(),
                                reversedGraph[i].end());

    /**
     * now graph is undirected
     */
    return directedGraph;
}

enum vertexStatus { WHITE, GRAY, BLACK };

/**
 * Depth First Search (DFS) for a Graph
 */
template <typename T>
vector<T> DFS(vector<vector<T>>& graph, unordered_map<T, vertexStatus>& visited,
              T start) {
    /**
     * if vertex is BLACK (we cannot go anywhere from it)
     * so we return empty vector
     */
    if (visited[start] == vertexStatus::BLACK) return {};

    visited[start] = vertexStatus::GRAY;

    vector<T> exitOrder;
    for (const T& neighbor : graph[start]) {
        if (visited[neighbor] == vertexStatus::WHITE) {
            vector<T> subResult = DFS(graph, visited, neighbor);
            exitOrder.insert(exitOrder.end(), subResult.begin(),
                             subResult.end());
        }
    }

    visited[start] = vertexStatus::BLACK;
    exitOrder.push_back(start);
    return exitOrder;
}

template <typename T>
vector<T> getExitOrder(vector<vector<T>>& graph) {
    /**
     * creating map [{0 -> status}, {1 -> status}, ...]
     * and fill it whith vertexStatus.WHITE
     */
    unordered_map<T, vertexStatus> visited;
    for (size_t vertex = 0; vertex < graph.size(); vertex++)
        visited[vertex] = vertexStatus::WHITE;

    vector<T> exitOrder;
    for (T i = 0; i < graph.size(); i++) {
        if (visited[i] == vertexStatus::WHITE) {
            vector<T> subResult = DFS(graph, visited, i);
            exitOrder.insert(exitOrder.end(), subResult.begin(),
                             subResult.end());
        }
    }

    return exitOrder;
}

/**
 * performing Kosaraju algorithm to check if adjacency list (graph) is strongly
 * connected
 */
template <typename T>
bool kosarajuAlgorithm(vector<vector<T>>& adjacencyList) {
    /**
     * Step 1:
     * Build the reversed graph
     */
    vector<vector<T>> reversed = reverseGraph(adjacencyList);
    if (false) printGraph(reversed);

    /**
     * Step 2:
     * First DFS to get the exit order of vertices
     */
    vector<T> exitOrder = getExitOrder(adjacencyList);

    // copy of ordered list of verteces to stack
    stack<T> orderStack;
    for (const T& vertex : exitOrder) orderStack.push(vertex);

    /**
     * Step 3:
     * Second DFS using the order from the stack
     */
    unordered_map<T, vertexStatus> visited;
    size_t componentsAmount = 0;

    while (!orderStack.empty()) {
        T current = orderStack.top();
        orderStack.pop();

        if (visited[current] == vertexStatus::WHITE) {
            // Strongly Connected Component
            vector<T> currentSCC = DFS(reversed, visited, current);

            if (true) {
                cout << "Component: ";
                for (const T& vertex : currentSCC) cout << vertex << " ";
                cout << endl;
            }

            componentsAmount++;
        }
    }

    if (componentsAmount > 1)
        cout << "Amount of components of strong connectivity: "
             << componentsAmount << endl;

    return componentsAmount == 1;
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
    vector<vector<vertexType>> undirectedGraph =
        initUndirectedGraph(adjacencyList);
    if (false) printGraph(undirectedGraph);

    vector<vertexType> visitResult =
        DFS(undirectedGraph, visited, (vertexType)0);
    bool isConnected = visitResult.size() == adjacencyList.size();

    printConclusion(isStronglyConnected, isConnected);
}

#endif
