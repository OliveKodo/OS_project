// tomergal40@gmail.com
#pragma once
#include <vector>
#include <random>

namespace graph {

struct Neighbor {
    int dest;
    int weight;
    Neighbor* next;
    Neighbor(int d, int w, Neighbor* n = nullptr) : dest(d), weight(w), next(n) {}
};

class Graph {
private:
    int numVertices;
    Neighbor** adjList;
    
    // Helper functions for Euler circuit algorithm
    void dfs(int vertex, std::vector<bool>& visited) const;
    void hierholzerDFS(int vertex, std::vector<int>& circuit, Graph& tempGraph) const;

public:
    Graph(int n);
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);
    void addEdge(int src, int dest, int weight = 1);
    void removeEdge(int src, int dest);
    void print_graph() const;
    int getNumVertices() const;
    Neighbor* getNeighbors(int vertex) const;
    bool hasEdge(int src, int dest) const;
    int getEdgeWeight(int src, int dest) const;
    void display() const;
    int getDegree(int vertex) const;
    int getNumEdges() const;
    bool isConnected() const;
    bool hasEulerCircuit() const;
    std::vector<int> findEulerCircuit() const;
    
    // Static function to generate random graph
    static Graph generateRandomGraph(int vertices, int edges, unsigned int seed);
    
    // Destructor - חייב להיות PUBLIC!
    ~Graph();
};

}