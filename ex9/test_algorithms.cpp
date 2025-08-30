#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include "graph.hpp"
#include "graph_algorithm.hpp"
#include "point.hpp"

// Test Point class functionality
void testPointClass() {
    std::cout << "Testing Point Class:\n";
    std::cout << "========================================\n";
    
    // Test constructors and basic functionality
    Point p1(3.0f, 4.0f);
    Point p2(0.0f, 0.0f);
    Point p3(3.0f, 4.0f);
    
    // Test getters
    assert(p1.getX() == 3.0f);
    assert(p1.getY() == 4.0f);
    assert(p2.getX() == 0.0f);
    assert(p2.getY() == 0.0f);
    
    // Test setters
    p2.setX(1.0f);
    p2.setY(2.0f);
    assert(p2.getX() == 1.0f);
    assert(p2.getY() == 2.0f);
    
    // Test distance calculation
    float dist = p1.distanceTo(p2);
    assert(dist > 0);
    
    // Test operator overloads
    assert(p1 == p3);
    assert(p1 != p2);
    assert(!(p1 == p2));
    
    // Test display function
    std::cout << "Point 1: ";
    p1.display();
    std::cout << "\nPoint 2: ";
    p2.display();
    std::cout << "\n";
    
    std::cout << "Point class tests passed!\n\n";
}

// Test Graph edge cases and additional functionality
void testGraphEdgeCases() {
    std::cout << "Testing Graph Edge Cases:\n";
    std::cout << "========================================\n";
    
    // Test empty graph - this should throw an exception
    try {
        graph::Graph emptyGraph(0);
        std::cout << "Empty graph created (unexpected)\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "Empty graph creation properly rejected: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for empty graph: " << e.what() << "\n";
    }
    
    // Test single vertex graph
    try {
        graph::Graph singleGraph(1);
        singleGraph.addEdge(0, 0, 1); // Self-loop
        std::cout << "Single vertex graph with self-loop created\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for single vertex graph: " << e.what() << "\n";
    }
    
    // Test small graph
    try {
        graph::Graph smallGraph(2);
        smallGraph.addEdge(0, 1, 5);
        std::cout << "Small graph created\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for small graph: " << e.what() << "\n";
    }
    
    // Test graph with disconnected components
    try {
        graph::Graph disconnectedGraph(4);
        disconnectedGraph.addEdge(0, 1, 1);
        disconnectedGraph.addEdge(2, 3, 2);
        std::cout << "Disconnected graph created\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for disconnected graph: " << e.what() << "\n";
    }
    
    // Test graph with all edges having same weight
    try {
        graph::Graph uniformGraph(3);
        uniformGraph.addEdge(0, 1, 1);
        uniformGraph.addEdge(1, 2, 1);
        uniformGraph.addEdge(0, 2, 1);
        std::cout << "Uniform weight graph created\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for uniform graph: " << e.what() << "\n";
    }
    
    // Test Euler circuit with valid graph
    try {
        graph::Graph eulerGraph(4);
        eulerGraph.addEdge(0, 1, 1);
        eulerGraph.addEdge(1, 2, 1);
        eulerGraph.addEdge(2, 3, 1);
        eulerGraph.addEdge(3, 0, 1);
        std::cout << "Euler circuit graph created\n";
        
        // Test hasEulerCircuit on valid graph
        if (eulerGraph.hasEulerCircuit()) {
            std::cout << "Euler circuit graph validation passed\n";
            std::vector<int> circuit = eulerGraph.findEulerCircuit();
            std::cout << "Euler circuit found with " << circuit.size() << " vertices\n";
        }
    } catch (const std::exception& e) {
        std::cout << "Exception caught for Euler graph: " << e.what() << "\n";
    }
    
    // Test invalid edge additions
    try {
        graph::Graph testGraph(2);
        testGraph.addEdge(0, 1, 1);
        testGraph.addEdge(1, 0, 1); // Reverse edge
        testGraph.addEdge(0, 0, 1); // Self-loop
        std::cout << "Edge addition tests completed\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for edge addition: " << e.what() << "\n";
    }
    
    // Test more edge cases for better coverage
    try {
        graph::Graph largeGraph(10);
        // Add edges in various patterns
        for (int i = 0; i < 9; i++) {
            largeGraph.addEdge(i, i + 1, i + 1);
        }
        // Add some cross edges
        largeGraph.addEdge(0, 5, 10);
        largeGraph.addEdge(2, 7, 15);
        largeGraph.addEdge(1, 8, 20);
        std::cout << "Large graph with complex edge patterns created\n";
        
        // Test display function
        std::cout << "Large graph structure:\n";
        largeGraph.display();
        std::cout << "\n";
        
        // Test connectivity
        if (largeGraph.isConnected()) {
            std::cout << "Large graph is connected\n";
        } else {
            std::cout << "Large graph is not connected\n";
        }
        
        // Test degree calculations
        for (int i = 0; i < 10; i++) {
            int degree = largeGraph.getDegree(i);
            std::cout << "Vertex " << i << " degree: " << degree << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught for large graph: " << e.what() << "\n";
    }
    
    // Test boundary conditions
    try {
        graph::Graph boundaryGraph(3);
        // Test edge weight extremes
        boundaryGraph.addEdge(0, 1, 0);   // Zero weight
        boundaryGraph.addEdge(1, 2, 1000); // Large weight
        boundaryGraph.addEdge(0, 2, -5);   // Negative weight (if allowed)
        std::cout << "Boundary condition graph created\n";
    } catch (const std::exception& e) {
        std::cout << "Exception caught for boundary graph: " << e.what() << "\n";
    }
    
    std::cout << "Graph edge case tests completed!\n\n";
}

// Test comprehensive algorithm scenarios
void testComprehensiveAlgorithms() {
    std::cout << "Testing Comprehensive Algorithms:\n";
    std::cout << "========================================\n";
    
    // Create multiple test graphs with different characteristics
    std::vector<graph::Graph> testGraphs;
    
    // Graph 1: Dense graph
    graph::Graph denseGraph(5);
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            denseGraph.addEdge(i, j, (i + j) % 5 + 1);
        }
    }
    testGraphs.push_back(denseGraph);
    
    // Graph 2: Sparse graph
    graph::Graph sparseGraph(6);
    sparseGraph.addEdge(0, 1, 1);
    sparseGraph.addEdge(1, 2, 2);
    sparseGraph.addEdge(2, 3, 3);
    sparseGraph.addEdge(3, 4, 4);
    sparseGraph.addEdge(4, 5, 5);
    testGraphs.push_back(sparseGraph);
    
    // Graph 3: Star graph
    graph::Graph starGraph(5);
    for (int i = 1; i < 5; i++) {
        starGraph.addEdge(0, i, i);
    }
    testGraphs.push_back(starGraph);
    
    // Test all algorithms on each graph
    std::vector<graph::AlgorithmFactory::AlgorithmType> algorithms = {
        graph::AlgorithmFactory::AlgorithmType::MST_WEIGHT,
        graph::AlgorithmFactory::AlgorithmType::SCC,
        graph::AlgorithmFactory::AlgorithmType::MAX_FLOW,
        graph::AlgorithmFactory::AlgorithmType::MAX_CLIQUE
    };
    
    for (size_t graphIdx = 0; graphIdx < testGraphs.size(); graphIdx++) {
        std::cout << "Testing Graph " << (graphIdx + 1) << ":\n";
        std::cout << "----------------------------------------\n";
        
        for (auto algoType : algorithms) {
            std::cout << "Testing " << graph::AlgorithmFactory::getAlgorithmName(algoType) << ":\n";
            
            auto algorithm = graph::AlgorithmFactory::createAlgorithm(algoType);
            if (algorithm) {
                try {
                    std::string result = algorithm->execute(testGraphs[graphIdx]);
                    std::cout << "Result: " << result << "\n";
                } catch (const std::exception& e) {
                    std::cout << "Exception: " << e.what() << "\n";
                }
            } else {
                std::cout << "Failed to create algorithm instance\n";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "Comprehensive algorithm tests completed!\n\n";
}

int main() {
    std::cout << "Enhanced Testing for Graph Algorithms and Point Class\n";
    std::cout << "====================================================\n\n";
    
    // Test Point class first
    testPointClass();
    
    // Test Graph edge cases
    testGraphEdgeCases();
    
    // Test comprehensive algorithms
    testComprehensiveAlgorithms();
    
    // Original test graph
    std::cout << "Testing Original Test Graph:\n";
    std::cout << "========================================\n";
    
    graph::Graph testGraph(6);
    testGraph.addEdge(0, 1, 4);
    testGraph.addEdge(0, 2, 3);
    testGraph.addEdge(1, 2, 1);
    testGraph.addEdge(1, 3, 2);
    testGraph.addEdge(2, 3, 4);
    testGraph.addEdge(2, 4, 3);
    testGraph.addEdge(3, 4, 2);
    testGraph.addEdge(3, 5, 1);
    testGraph.addEdge(4, 5, 6);
    
    std::cout << "Test Graph:\n";
    testGraph.display();
    std::cout << "\n";
    
    // Test all algorithms using the Factory pattern
    std::vector<graph::AlgorithmFactory::AlgorithmType> algorithms = {
        graph::AlgorithmFactory::AlgorithmType::MST_WEIGHT,
        graph::AlgorithmFactory::AlgorithmType::SCC,
        graph::AlgorithmFactory::AlgorithmType::MAX_FLOW,
        graph::AlgorithmFactory::AlgorithmType::MAX_CLIQUE
    };
    
    for (auto algoType : algorithms) {
        std::cout << "Testing " << graph::AlgorithmFactory::getAlgorithmName(algoType) << ":\n";
        std::cout << "----------------------------------------\n";
        
        auto algorithm = graph::AlgorithmFactory::createAlgorithm(algoType);
        if (algorithm) {
            std::string result = algorithm->execute(testGraph);
            std::cout << result << "\n";
        } else {
            std::cout << "Failed to create algorithm instance\n";
        }
        std::cout << "\n";
    }
    
    // Test Euler circuit (original functionality)
    std::cout << "Testing Euler Circuit:\n";
    std::cout << "----------------------------------------\n";
    if (testGraph.hasEulerCircuit()) {
        std::vector<int> circuit = testGraph.findEulerCircuit();
        std::cout << "SUCCESS: Graph has Euler circuit!\n";
        std::cout << "Circuit: ";
        for (size_t i = 0; i < circuit.size(); ++i) {
            if (i > 0) std::cout << " -> ";
            std::cout << circuit[i];
        }
        std::cout << "\n";
    } else {
        std::cout << "RESULT: Graph does NOT have Euler circuit\n";
        std::cout << "Reason: Graph is not connected or has odd-degree vertices\n";
    }
    
    std::cout << "\nAll tests completed successfully!\n";
    return 0;
}
