#include "graph_algorithm.hpp"
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <climits>
#include <sstream>

namespace graph {

// MST Weight Algorithm Implementation
class MSTWeightAlgorithm : public GraphAlgorithm {
public:
    std::string execute(const Graph& graph) override {
        int n = graph.getNumVertices();
        if (n == 0) return "Graph is empty";
        
        // Prim's algorithm for MST
        std::vector<bool> visited(n, false);
        std::vector<int> key(n, INT_MAX);
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, 
                           std::greater<std::pair<int, int>>> pq;
        
        int mstWeight = 0;
        int startVertex = 0;
        
        key[startVertex] = 0;
        pq.push({0, startVertex});
        
        while (!pq.empty()) {
            int u = pq.top().second;
            int weight = pq.top().first;
            pq.pop();
            
            if (visited[u]) continue;
            
            visited[u] = true;
            mstWeight += weight;
            
            Neighbor* current = graph.getNeighbors(u);
            while (current != nullptr) {
                int v = current->dest;
                int w = current->weight;
                
                if (!visited[v] && w < key[v]) {
                    key[v] = w;
                    pq.push({w, v});
                }
                current = current->next;
            }
        }
        
        // Check if MST covers all vertices
        for (int i = 0; i < n; i++) {
            if (graph.getDegree(i) > 0 && !visited[i]) {
                return "Graph is not connected - MST weight: " + std::to_string(mstWeight);
            }
        }
        
        return "MST Weight: " + std::to_string(mstWeight);
    }
    
    std::string getName() const override {
        return "MST Weight";
    }
};

// Strongly Connected Components Algorithm Implementation
class SCCAlgorithm : public GraphAlgorithm {
private:
    void dfs1(const Graph& graph, int v, std::vector<bool>& visited, std::vector<int>& order) {
        visited[v] = true;
        Neighbor* current = graph.getNeighbors(v);
        while (current != nullptr) {
            if (!visited[current->dest]) {
                dfs1(graph, current->dest, visited, order);
            }
            current = current->next;
        }
        order.push_back(v);
    }
    
    void dfs2(const Graph& graph, int v, std::vector<bool>& visited, std::vector<int>& component) {
        visited[v] = true;
        component.push_back(v);
        Neighbor* current = graph.getNeighbors(v);
        while (current != nullptr) {
            if (!visited[current->dest]) {
                dfs2(graph, current->dest, visited, component);
            }
            current = current->next;
        }
    }
    
public:
    std::string execute(const Graph& graph) override {
        int n = graph.getNumVertices();
        if (n == 0) return "Graph is empty";
        
        std::vector<bool> visited(n, false);
        std::vector<int> order;
        
        // First DFS to get topological order
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                dfs1(graph, i, visited, order);
            }
        }
        
        // Create transpose graph
        Graph transpose(n);
        for (int i = 0; i < n; i++) {
            Neighbor* current = graph.getNeighbors(i);
            while (current != nullptr) {
                transpose.addEdge(current->dest, i, current->weight);
                current = current->next;
            }
        }
        
        // Second DFS on transpose graph
        std::fill(visited.begin(), visited.end(), false);
        std::vector<std::vector<int>> sccs;
        
        for (int i = order.size() - 1; i >= 0; i--) {
            int v = order[i];
            if (!visited[v]) {
                std::vector<int> component;
                dfs2(transpose, v, visited, component);
                sccs.push_back(component);
            }
        }
        
        std::stringstream result;
        result << "Found " << sccs.size() << " Strongly Connected Components:\n";
        for (size_t i = 0; i < sccs.size(); i++) {
            result << "SCC " << (i + 1) << ": {";
            for (size_t j = 0; j < sccs[i].size(); j++) {
                if (j > 0) result << ", ";
                result << sccs[i][j];
            }
            result << "}\n";
        }
        
        return result.str();
    }
    
    std::string getName() const override {
        return "Strongly Connected Components";
    }
};

// Max Flow Algorithm Implementation (Ford-Fulkerson)
class MaxFlowAlgorithm : public GraphAlgorithm {
private:
    bool bfs(const std::vector<std::vector<int>>& residual, int source, int sink, 
             std::vector<int>& parent, int n) {
        std::vector<bool> visited(n, false);
        std::queue<int> q;
        
        q.push(source);
        visited[source] = true;
        parent[source] = -1;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int v = 0; v < n; v++) {
                if (!visited[v] && residual[u][v] > 0) {
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                }
            }
        }
        
        return visited[sink];
    }
    
public:
    std::string execute(const Graph& graph) override {
        int n = graph.getNumVertices();
        if (n < 2) return "Graph needs at least 2 vertices for max flow";
        
        int source = 0;
        int sink = n - 1;
        
        // Create adjacency matrix for residual graph
        std::vector<std::vector<int>> residual(n, std::vector<int>(n, 0));
        
        // Fill residual graph
        for (int i = 0; i < n; i++) {
            Neighbor* current = graph.getNeighbors(i);
            while (current != nullptr) {
                residual[i][current->dest] = current->weight;
                current = current->next;
            }
        }
        
        int maxFlow = 0;
        std::vector<int> parent(n);
        
        // Ford-Fulkerson algorithm
        while (bfs(residual, source, sink, parent, n)) {
            int pathFlow = INT_MAX;
            
            // Find minimum residual capacity along the path
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                pathFlow = std::min(pathFlow, residual[u][v]);
            }
            
            // Update residual capacities
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                residual[u][v] -= pathFlow;
                residual[v][u] += pathFlow;
            }
            
            maxFlow += pathFlow;
        }
        
        return "Max Flow from node 0 to node " + std::to_string(n-1) + ": " + std::to_string(maxFlow);
    }
    
    std::string getName() const override {
        return "Max Flow";
    }
};

// Max Clique Algorithm Implementation (Bron-Kerbosch)
class MaxCliqueAlgorithm : public GraphAlgorithm {
private:
    void bronKerbosch(std::vector<int>& R, std::vector<int>& P, std::vector<int>& X, 
                      const Graph& graph, std::vector<int>& maxClique, size_t& maxSize) {
        if (P.empty() && X.empty()) {
            if (R.size() > maxSize) {
                maxSize = R.size();
                maxClique = R;
            }
            return;
        }
        
        std::vector<int> P_copy = P;
        for (int v : P_copy) {
            std::vector<int> R_new = R;
            R_new.push_back(v);
            
            std::vector<int> P_new, X_new;
            
            // P ∩ N(v)
            for (int u : P) {
                if (graph.hasEdge(v, u)) {
                    P_new.push_back(u);
                }
            }
            
            // X ∩ N(v)
            for (int u : X) {
                if (graph.hasEdge(v, u)) {
                    X_new.push_back(u);
                }
            }
            
            bronKerbosch(R_new, P_new, X_new, graph, maxClique, maxSize);
            
            // Move v from P to X
            P.erase(std::remove(P.begin(), P.end(), v), P.end());
            X.push_back(v);
        }
    }
    
public:
    std::string execute(const Graph& graph) override {
        int n = graph.getNumVertices();
        if (n == 0) return "Graph is empty";
        
        std::vector<int> R, P, X;
        for (int i = 0; i < n; i++) {
            P.push_back(i);
        }
        
        std::vector<int> maxClique;
        size_t maxSize = 0;
        
        bronKerbosch(R, P, X, graph, maxClique, maxSize);
        
        std::stringstream result;
        result << "Max Clique Size: " << maxSize << "\n";
        result << "Max Clique Vertices: {";
        for (size_t i = 0; i < maxClique.size(); i++) {
            if (i > 0) result << ", ";
            result << maxClique[i];
        }
        result << "}";
        
        return result.str();
    }
    
    std::string getName() const override {
        return "Max Clique";
    }
};

// Factory implementation
std::unique_ptr<GraphAlgorithm> AlgorithmFactory::createAlgorithm(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::MST_WEIGHT:
            return std::make_unique<MSTWeightAlgorithm>();
        case AlgorithmType::SCC:
            return std::make_unique<SCCAlgorithm>();
        case AlgorithmType::MAX_FLOW:
            return std::make_unique<MaxFlowAlgorithm>();
        case AlgorithmType::MAX_CLIQUE:
            return std::make_unique<MaxCliqueAlgorithm>();
        default:
            return nullptr;
    }
}

std::string AlgorithmFactory::getAlgorithmName(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::MST_WEIGHT:
            return "MST_WEIGHT";
        case AlgorithmType::SCC:
            return "SCC";
        case AlgorithmType::MAX_FLOW:
            return "MAX_FLOW";
        case AlgorithmType::MAX_CLIQUE:
            return "MAX_CLIQUE";
        default:
            return "UNKNOWN";
    }
}

} // namespace graph
