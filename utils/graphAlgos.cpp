#include <bitset>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

using namespace std;

#ifndef GRAPH_ALORYTHMS
#define GRAPH_ALORYTHMS

// =========
// structs:
// =========

/**
 * required struct for correct work of Kosaraju Algorithm
 */
enum vertexStatus { WHITE, GRAY, BLACK };

// ========== =====
// executable part:
// ========== =====

/**
 * reverse graph's edges
 *
 * Example:
 * 1 --> 1
 * 2 --> 1
 * 3 --> 2
 *
 * Becomes:
 * 1 --> 1
 * 2 --> 3
 * 1 --> 2
 */
template <typename T>
vector<vector<T>> reverseGraph(vector<vector<T>> graph) {
    vector<vector<T>> reversedGraph(graph.size());

    for (size_t i = 0; i < graph.size(); ++i)
        for (const T& neighbor : graph[i]) reversedGraph[neighbor].push_back(i);

    return reversedGraph;
}

/**
 * creates undirected graph from directed
 * by uniting initial graph and it's reversed copy
 */
template <typename T>
vector<vector<T>> dirToUndirGraph(vector<vector<T>> directedGraph) {
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

    // @COMMENTLINE
    // cout << "Exit order: " << endl;
    // for (size_t k = 0; k < exitOrder.size(); k++) {
    //     cout << exitOrder[k] << " ";
    // }
    // cout << endl;

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

#endif
