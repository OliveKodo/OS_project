#pragma once
#include "graph.hpp"
#include <string>
#include <memory>

namespace graph {

// Strategy pattern interface for graph algorithms
class GraphAlgorithm {
public:
    virtual ~GraphAlgorithm() = default;
    virtual std::string execute(const Graph& graph) = 0;
    virtual std::string getName() const = 0;
};

// Factory pattern for creating algorithm instances
class AlgorithmFactory {
public:
    enum class AlgorithmType {
        MST_WEIGHT,
        SCC,
        MAX_FLOW,
        MAX_CLIQUE
    };
    
    static std::unique_ptr<GraphAlgorithm> createAlgorithm(AlgorithmType type);
    static std::string getAlgorithmName(AlgorithmType type);
};

} // namespace graph
