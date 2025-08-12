// tomergal40@gmail.com
#pragma once

namespace graph {

    struct Neighbor
    {
    int dest;
    int weight;
    Neighbor* next;

        Neighbor(int d, int w, Neighbor* n = nullptr) : dest(d), weight(w), next(n) {}    
        };

    class Graph {
    private:
        int numVertices;
        Neighbor** adjList;    
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
    
    
    ~Graph();
    };
    
}