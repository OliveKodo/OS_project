// tomergal40@gmail.com
#include "graph.hpp"
#include <iostream> // Only for printing

namespace graph {
    
    // Constructor
    Graph::Graph(int n) : numVertices(n) {
        if (n <= 0) {
            throw std::invalid_argument("Number of vertices must be positive");
        }
        
        // Allocate adjacency lists array
        adjList = new Neighbor*[n];
        for (int i = 0; i < n; i++) {
            adjList[i] = nullptr; // Initialize empty lists
        }
    }
    
    // Copy constructor
    Graph::Graph(const Graph& other) : numVertices(other.numVertices) {
        // Allocate adjacency lists array
        adjList = new Neighbor*[numVertices];
        
        // Copy adjacency lists
        for (int i = 0; i < numVertices; i++) {
            // Initialize empty list
            adjList[i] = nullptr;
            
            // Copy the linked list
            Neighbor* current = other.adjList[i];
            Neighbor** tail = &adjList[i]; // Pointer to the tail of the new list
            
            while (current != nullptr) {
                // Create new node
                *tail = new Neighbor(current->dest, current->weight);
                
                // Advance pointers
                tail = &((*tail)->next);
                current = current->next;
            }
        }
    }
    
    // Assignment operator
    Graph& Graph::operator=(const Graph& other) {
        if (this != &other) { // Check self-assignment
            // Free previous memory
            for (int i = 0; i < numVertices; i++) {
                Neighbor* current = adjList[i];
                while (current != nullptr) {
                    Neighbor* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] adjList;
            
            // Allocate and copy new data
            numVertices = other.numVertices;
            adjList = new Neighbor*[numVertices];
            
            for (int i = 0; i < numVertices; i++) {
                adjList[i] = nullptr;
                
                Neighbor* current = other.adjList[i];
                Neighbor** tail = &adjList[i];
                
                while (current != nullptr) {
                    *tail = new Neighbor(current->dest, current->weight);
                    
                    tail = &((*tail)->next);
                    current = current->next;
                }
            }
        }
        return *this;
    }
    
    // Add edge
    void Graph::addEdge(int src, int dest, int weight) {
        // Validate vertex indices
        if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
            throw std::out_of_range("Vertex index out of range");
        }
        
        // Prevent self-loops (optional, depending on requirements)
        if (src == dest) {
            throw std::invalid_argument("Self loops are not allowed");
        }
        
        // Check if edge already exists
        if (hasEdge(src, dest)) {
            // Update weight if edge exists
            Neighbor* current = adjList[src];
            while (current != nullptr && current->dest != dest) {
                current = current->next;
            }
            if (current != nullptr) {
                current->weight = weight;
            }
            
            // Update in the other direction (undirected graph)
            current = adjList[dest];
            while (current != nullptr && current->dest != src) {
                current = current->next;
            }
            if (current != nullptr) {
                current->weight = weight;
            }
        } else {
            // Add new edge in both directions (undirected graph)
            adjList[src] = new Neighbor(dest, weight, adjList[src]);
            adjList[dest] = new Neighbor(src, weight, adjList[dest]);
        }
    }
    
    // Remove edge
    void Graph::removeEdge(int src, int dest) {
        // Validate vertex indices
        if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
            throw std::out_of_range("Vertex index out of range");
        }
        
        // Check if edge exists before trying to remove it
        if (!hasEdge(src, dest)) {
            throw std::runtime_error("Edge not found");
        }
        
        // Remove from src's adjacency list
        Neighbor** current = &adjList[src];
        while (*current != nullptr) {
            if ((*current)->dest == dest) {
                // Found the edge - remove it
                Neighbor* temp = *current;
                *current = (*current)->next;
                delete temp;
                break;
            }
            current = &((*current)->next);
        }
        
        // Remove from dest's adjacency list
        current = &adjList[dest];
        while (*current != nullptr) {
            if ((*current)->dest == src) {
                // Found the edge - remove it
                Neighbor* temp = *current;
                *current = (*current)->next;
                delete temp;
                break;
            }
            current = &((*current)->next);
        }
    }
    
    // Print the graph
    void Graph::print_graph() const {
        for (int i = 0; i < numVertices; i++) {
            std::cout << "Vertex " << i << " -> ";
            Neighbor* current = adjList[i];
            
            while (current != nullptr) {
                std::cout << "(" << current->dest << ", weight: " << current->weight << ") ";
                current = current->next;
            }
            
            std::cout << std::endl;
        }
    }
    
    // Access functions
    int Graph::getNumVertices() const {
        return numVertices;
    }
    
    Neighbor* Graph::getNeighbors(int vertex) const {
        if (vertex < 0 || vertex >= numVertices) {
            throw std::out_of_range("Vertex index out of range");
        }
        return adjList[vertex];
    }
    
    // Check if edge exists
    bool Graph::hasEdge(int src, int dest) const {
        if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
            throw std::out_of_range("Vertex index out of range");
        }
        
        Neighbor* current = adjList[src];
        while (current != nullptr) {
            if (current->dest == dest) {
                return true;
            }
            current = current->next;
        }
        
        return false;
    }
    
    // Get edge weight
    int Graph::getEdgeWeight(int src, int dest) const {
        if (src < 0 || src >= numVertices || dest < 0 || dest >= numVertices) {
            throw std::out_of_range("Vertex index out of range");
        }
        
        Neighbor* current = adjList[src];
        while (current != nullptr) {
            if (current->dest == dest) {
                return current->weight;
            }
            current = current->next;
        }
        
        throw std::runtime_error("Edge not found");
    }
    
    // Destructor
    Graph::~Graph() {
        for (int i = 0; i < numVertices; i++) {
            Neighbor* current = adjList[i];
            while (current != nullptr) {
                Neighbor* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] adjList;
    }
}