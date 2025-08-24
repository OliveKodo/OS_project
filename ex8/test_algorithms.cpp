#include <iostream>
#include <memory>
#include "graph.hpp"
#include "graph_algorithm.hpp"

int main() {
    std::cout << "Testing Graph Algorithms with Factory and Strategy Patterns\n";
    std::cout << "========================================================\n\n";
    
    // Create a test graph
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
    
    return 0;
}
