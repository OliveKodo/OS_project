#include "graph.hpp"
#include "point.hpp"
#include <getopt.h>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <vector>


void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -v, --vertices NUM    Number of vertices (default: 5)\n";
    std::cout << "  -e, --edges NUM       Number of edges (default: 6)\n";
    std::cout << "  -s, --seed NUM        Random seed (default: current time)\n";
    std::cout << "  -h, --help           Show this help message\n";
    std::cout << "  -t, --test           Run test with predefined graphs\n";
}

void runEulerTests() {
   using namespace graph;
   
   std::cout << "=== TESTING POINT CLASS ===\n";
   Point p1(3.0, 4.0);
   Point p2(6.0, 8.0);
   Point p3;
   std::cout << "Point distance test: " << p1.distanceTo(p2) << "\n";
   std::cout << "Point coordinates: (" << p1.getX() << ", " << p1.getY() << ")\n";
   
   p1.setX(10.0);
   p1.setY(20.0);
   p1.display();
   std::cout << " vs ";
   p2.display();
   std::cout << "\nPoints equal? " << (p1 == p2 ? "Yes" : "No") << "\n";
   std::cout << "Points different? " << (p1 != p2 ? "Yes" : "No") << "\n";
   std::cout << "✓ Point class tested\n\n";
   
   std::cout << "=== EULER CIRCUIT TESTS (Parts 1-2) ===\n\n";
   
   // Test 1: Graph with Euler circuit (all vertices have even degree)
   std::cout << "Test 1: Graph with Euler circuit\n";
   Graph testGraph(4);
   testGraph.addEdge(0, 1);
   testGraph.addEdge(1, 2);
   testGraph.addEdge(2, 3);
   testGraph.addEdge(3, 0);
   testGraph.addEdge(0, 2);
   testGraph.addEdge(1, 3);
   
   std::cout << "Graph structure:\n";
   testGraph.display();
   
   std::cout << "\nVertex degrees:\n";
   for (int i = 0; i < testGraph.getNumVertices(); i++) {
       std::cout << "Vertex " << i << ": degree " << testGraph.getDegree(i) << "\n";
   }
   
   std::cout << "\nChecking for Euler circuit...\n";
   if (testGraph.hasEulerCircuit()) {
       std::cout << "✓ Euler circuit exists!\n";
       std::vector<int> circuit = testGraph.findEulerCircuit();
       std::cout << "Euler circuit: ";
       for (size_t i = 0; i < circuit.size(); i++) {
           std::cout << circuit[i];
           if (i < circuit.size() - 1) std::cout << " → ";
       }
       std::cout << "\n";
   } else {
       std::cout << "✗ No Euler circuit exists.\n";
   }
   
   std::cout << "\n" << std::string(50, '=') << "\n\n";
   
   // Test 2: Graph without Euler circuit (odd degree vertices)
   std::cout << "Test 2: Graph without Euler circuit (odd degrees)\n";
   Graph testGraph2(3);
   testGraph2.addEdge(0, 1);
   testGraph2.addEdge(1, 2);
   
   std::cout << "Graph structure:\n";
   testGraph2.display();
   
   std::cout << "\nVertex degrees:\n";
   for (int i = 0; i < testGraph2.getNumVertices(); i++) {
       std::cout << "Vertex " << i << ": degree " << testGraph2.getDegree(i) << "\n";
   }
   
   std::cout << "\nChecking for Euler circuit...\n";
   if (testGraph2.hasEulerCircuit()) {
       std::cout << "✓ Euler circuit exists!\n";
       std::vector<int> circuit = testGraph2.findEulerCircuit();
       std::cout << "Euler circuit: ";
       for (size_t i = 0; i < circuit.size(); i++) {
           std::cout << circuit[i];
           if (i < circuit.size() - 1) std::cout << " → ";
       }
       std::cout << "\n";
   } else {
       std::cout << "✗ No Euler circuit exists.\n";
       std::cout << "Reason: Not all vertices have even degree.\n";
   }
   
   std::cout << "\n" << std::string(50, '=') << "\n\n";
   
   // Test 3: Disconnected graph
   std::cout << "Test 3: Disconnected graph\n";
   Graph testGraph3(4);
   testGraph3.addEdge(0, 1);
   testGraph3.addEdge(2, 3);
   
   std::cout << "Graph structure:\n";
   testGraph3.display();
   
   std::cout << "\nVertex degrees:\n";
   for (int i = 0; i < testGraph3.getNumVertices(); i++) {
       std::cout << "Vertex " << i << ": degree " << testGraph3.getDegree(i) << "\n";
   }
   
   std::cout << "\nChecking for Euler circuit...\n";
   if (testGraph3.hasEulerCircuit()) {
       std::cout << "✓ Euler circuit exists!\n";
       std::vector<int> circuit = testGraph3.findEulerCircuit();
       std::cout << "Euler circuit: ";
       for (size_t i = 0; i < circuit.size(); i++) {
           std::cout << circuit[i];
           if (i < circuit.size() - 1) std::cout << " → ";
       }
       std::cout << "\n";
   } else {
       std::cout << "✗ No Euler circuit exists.\n";
       std::cout << "Reason: Graph is not connected.\n";
   }
   
   std::cout << "\n" << std::string(50, '=') << "\n\n";
   
   // Test 4: Single vertex (trivial case)
   std::cout << "Test 4: Single vertex (trivial case)\n";
   Graph testGraph4(1);
   
   std::cout << "Graph structure:\n";
   testGraph4.display();
   
   std::cout << "\nChecking for Euler circuit...\n";
   if (testGraph4.hasEulerCircuit()) {
       std::cout << "✓ Euler circuit exists (trivial case)!\n";
       std::vector<int> circuit = testGraph4.findEulerCircuit();
       if (circuit.empty()) {
           std::cout << "Empty circuit (no edges).\n";
       } else {
           std::cout << "Euler circuit: ";
           for (size_t i = 0; i < circuit.size(); i++) {
               std::cout << circuit[i];
               if (i < circuit.size() - 1) std::cout << " → ";
           }
           std::cout << "\n";
       }
   } else {
       std::cout << "✗ No Euler circuit exists.\n";
   }
   
   std::cout << "\n" << std::string(50, '=') << "\n\n";
   
   // הוסף את הטסטים החדשים כאן:
   std::cout << "=== ERROR HANDLING TESTS ===\n";
   
   // Test invalid vertices
   try {
       Graph invalidGraph(-1);
   } catch (const std::invalid_argument& e) {
       std::cout << "✓ Caught negative vertices error: " << e.what() << "\n";
   }
   
   // Test invalid edge - out of range
   try {
       Graph testGraph(3);
       testGraph.addEdge(0, 5, 1);  // Out of range
   } catch (const std::out_of_range& e) {
       std::cout << "✓ Caught out of range error: " << e.what() << "\n";
   }
   
   // Test self loop
   try {
       Graph testGraph(3);
       testGraph.addEdge(0, 0, 1);  // Self loop
   } catch (const std::invalid_argument& e) {
       std::cout << "✓ Caught self loop error: " << e.what() << "\n";
   }
   
   // Test getEdgeWeight
   Graph testGraph5(3);
   testGraph5.addEdge(0, 1, 5);
   std::cout << "Edge weight (0,1): " << testGraph5.getEdgeWeight(0, 1) << "\n";
   
   // Test assignment operator
   Graph graph1(2);
   graph1.addEdge(0, 1);
   Graph graph2(3);
   graph2 = graph1;
   std::cout << "✓ Assignment operator tested\n";
   
   // Test updating existing edge
   Graph testGraph6(3);
   testGraph6.addEdge(0, 1, 1);
   testGraph6.addEdge(0, 1, 5);  // Update existing edge
   std::cout << "✓ Edge update tested\n";
   
   // Test removing non-existent edge
   try {
       Graph testGraph7(3);
       testGraph7.removeEdge(0, 1);  // Edge doesn't exist
   } catch (const std::runtime_error& e) {
       std::cout << "✓ Caught remove non-existent edge error: " << e.what() << "\n";
   }
   
   std::cout << "✓ All error handling tests completed\n\n";
}

void analyzeGraph(const graph::Graph& g) {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "GRAPH ANALYSIS\n";
    std::cout << std::string(50, '=') << "\n";
    
    g.display();
    
    std::cout << "\nVertex degrees:\n";
    bool allEven = true;
    for (int i = 0; i < g.getNumVertices(); i++) {
        int degree = g.getDegree(i);
        std::cout << "Vertex " << i << ": degree " << degree;
        if (degree % 2 != 0) {
            std::cout << " (odd)";
            allEven = false;
        } else if (degree > 0) {
            std::cout << " (even)";
        }
        std::cout << "\n";
    }
    
    std::cout << "\nConnectivity: " << (g.isConnected() ? "Connected" : "Disconnected") << "\n";
    std::cout << "All degrees even: " << (allEven ? "Yes" : "No") << "\n";
    
    std::cout << "\n" << std::string(30, '-') << "\n";
    std::cout << "EULER CIRCUIT ANALYSIS\n";
    std::cout << std::string(30, '-') << "\n";
    
    if (g.hasEulerCircuit()) {
        std::cout << "✓ Euler circuit EXISTS!\n";
        std::cout << "Finding Euler circuit...\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<int> circuit = g.findEulerCircuit();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Euler circuit found in " << duration.count() << " microseconds:\n";
        std::cout << "Circuit: ";
        for (size_t i = 0; i < circuit.size(); i++) {
            std::cout << circuit[i];
            if (i < circuit.size() - 1) std::cout << " → ";
        }
        std::cout << "\n";
        std::cout << "Circuit length: " << circuit.size() << " vertices\n";
    } else {
        std::cout << "✗ No Euler circuit exists\n";
        if (!g.isConnected()) {
            std::cout << "Reason: Graph is not connected\n";
        } else if (!allEven) {
            std::cout << "Reason: Not all vertices have even degree\n";
        }
    }
    
    std::cout << std::string(50, '=') << "\n";
}

int main(int argc, char* argv[]) {
    int vertices = 5;
    int edges = 6;
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    bool runTestMode = false;
    
    // Long options structure
    static struct option long_options[] = {
        {"vertices", required_argument, 0, 'v'},
        {"edges", required_argument, 0, 'e'},
        {"seed", required_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {"test", no_argument, 0, 't'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    // Parse command line arguments
    while ((c = getopt_long(argc, argv, "v:e:s:ht", long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
                vertices = std::atoi(optarg);
                if (vertices <= 0) {
                    std::cerr << "Error: Number of vertices must be positive\n";
                    return 1;
                }
                break;
            case 'e':
                edges = std::atoi(optarg);
                if (edges < 0) {
                    std::cerr << "Error: Number of edges must be non-negative\n";
                    return 1;
                }
                break;
            case 's':
                seed = std::atoi(optarg);
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            case 't':
                runTestMode = true;
                break;
            case '?':
                std::cerr << "Unknown option. Use -h for help.\n";
                return 1;
            default:
                return 1;
        }
    }
    
    if (runTestMode) {
        runEulerTests();
        return 0;
    }
    
    // Validate parameters
    int maxPossibleEdges = (vertices * (vertices - 1)) / 2;
    if (edges > maxPossibleEdges) {
        std::cout << "Warning: Requested " << edges << " edges, but maximum possible is " 
                  << maxPossibleEdges << " for " << vertices << " vertices.\n";
        std::cout << "Setting edges to maximum possible value.\n";
        edges = maxPossibleEdges;
    }
    
    try {
        std::cout << "=== RANDOM GRAPH EULER CIRCUIT ANALYSIS ===\n\n";
        std::cout << "Generating random graph...\n";
        std::cout << "Parameters:\n";
        std::cout << "  Vertices: " << vertices << "\n";
        std::cout << "  Edges: " << edges << "\n";
        std::cout << "  Seed: " << seed << "\n\n";
        
        // Generate random graph using static method
        graph::Graph randomGraph = graph::Graph::generateRandomGraph(vertices, edges, seed);
        
        // Analyze the generated graph
        analyzeGraph(randomGraph);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}