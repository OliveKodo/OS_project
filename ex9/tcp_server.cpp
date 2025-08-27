#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>
#include <sstream>
#include "graph.hpp"
#include "graph_algorithm.hpp"

#define BACKLOG 5
#define PIPELINE_STAGES 6  // Request Handler, MST, SCC, MAX_FLOW, MAX_CLIQUE, Response Sender

// Global variables for server control
volatile sig_atomic_t running = 1;
int listen_socket_fd = -1;

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

// Utility function to trim whitespace from strings
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Pipeline stage data structure
struct PipelineData {
    int client_fd;
    std::string client_ip;
    std::string request;
    std::string algorithm;
    std::unique_ptr<graph::Graph> graph;
    std::string result;
    std::chrono::high_resolution_clock::time_point start_time;
    
    PipelineData(int fd, const std::string& ip, const std::string& req) 
        : client_fd(fd), client_ip(ip), request(req), graph(nullptr), start_time(std::chrono::high_resolution_clock::now()) {}
};

// Pipeline Pattern Implementation
class PipelineServer {
private:
    // Queues for each pipeline stage
    std::queue<std::shared_ptr<PipelineData>> request_queue;
    std::queue<std::shared_ptr<PipelineData>> mst_queue;
    std::queue<std::shared_ptr<PipelineData>> scc_queue;
    std::queue<std::shared_ptr<PipelineData>> max_flow_queue;
    std::queue<std::shared_ptr<PipelineData>> max_clique_queue;
    std::queue<std::shared_ptr<PipelineData>> response_queue;
    
    // Mutexes for each queue
    std::mutex request_mutex, mst_mutex, scc_mutex, max_flow_mutex, max_clique_mutex, response_mutex;
    
    // Condition variables for each queue
    std::condition_variable request_cv, mst_cv, scc_cv, max_flow_cv, max_clique_cv, response_cv;
    
    // Active objects (threads) for each pipeline stage
    std::vector<std::thread> pipeline_threads;
    
    // Statistics
    std::atomic<int> total_requests{0};
    std::atomic<int> completed_requests{0};
    
public:
    PipelineServer() {
        std::cout << "Creating Pipeline Server with " << PIPELINE_STAGES << " stages\n";
        
        // Create active objects for each pipeline stage
        // Multiple request handlers for true concurrency
        pipeline_threads.emplace_back(&PipelineServer::requestHandler, this, 0);
        pipeline_threads.emplace_back(&PipelineServer::requestHandler, this, 0); // Second request handler
        pipeline_threads.emplace_back(&PipelineServer::requestHandler, this, 0); // Third request handler
        
        pipeline_threads.emplace_back(&PipelineServer::mstProcessor, this, 1);
        pipeline_threads.emplace_back(&PipelineServer::sccProcessor, this, 2);
        pipeline_threads.emplace_back(&PipelineServer::maxFlowProcessor, this, 3);
        pipeline_threads.emplace_back(&PipelineServer::maxCliqueProcessor, this, 4);
        pipeline_threads.emplace_back(&PipelineServer::responseSender, this, 5);
        
        std::cout << "Pipeline stages created:\n";
        std::cout << "  0: Request Handler (3 threads for concurrency)\n";
        std::cout << "  1: MST Weight Processor\n";
        std::cout << "  2: SCC Processor\n";
        std::cout << "  3: Max Flow Processor\n";
        std::cout << "  4: Max Clique Processor\n";
        std::cout << "  5: Response Sender\n";
    }
    
    ~PipelineServer() {
        shutdown();
    }
    
    // Add new connection to the pipeline
    void addConnection(int client_fd, const std::string& client_ip) {
        // Create a connection handler that will read requests with timeout
        std::thread([this, client_fd, client_ip]() {
            this->handleConnection(client_fd, client_ip);
        }).detach();
    }
    
    // Add new request to the pipeline
    void addRequest(int client_fd, const std::string& client_ip, const std::string& request) {
        auto data = std::make_shared<PipelineData>(client_fd, client_ip, request);
        total_requests++;
        
        {
            std::lock_guard<std::mutex> lock(request_mutex);
            request_queue.push(data);
            std::cout << "Request added to pipeline. Queue size: " << request_queue.size() 
                      << ", Total requests: " << total_requests << " from " << client_ip << std::endl;
        }
        request_cv.notify_all(); // Wake up ALL waiting request handlers
    }
    
    // Shutdown the pipeline
    void shutdown() {
        std::cout << "Shutting down Pipeline Server...\n";
        running = 0;
        
        // Wake up all waiting threads
        request_cv.notify_all();
        mst_cv.notify_all();
        scc_cv.notify_all();
        max_flow_cv.notify_all();
        max_clique_cv.notify_all();
        response_cv.notify_all();
        
        // Join all pipeline threads
        for (auto& thread : pipeline_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        std::cout << "All pipeline threads finished\n";
    }
    
    // Get statistics
    void printStats() {
        std::cout << "\n=== PIPELINE STATISTICS ===\n";
        std::cout << "Total requests: " << total_requests << "\n";
        std::cout << "Completed requests: " << completed_requests << "\n";
        std::cout << "Pending requests: " << (total_requests - completed_requests) << "\n";
        std::cout << "==========================\n\n";
    }
    
    // Handle individual connection with timeout
    void handleConnection(int client_fd, const std::string& client_ip) {
        std::cout << "Connection handler started for " << client_ip << std::endl;
        
        // Use select to wait for data with timeout
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);
        
        struct timeval tv;
        tv.tv_sec = 30;  // 30 second timeout
        tv.tv_usec = 0;
        
        int activity = select(client_fd + 1, &readfds, NULL, NULL, &tv);
        
        if (activity < 0) {
            perror("select in connection handler");
            close(client_fd);
            return;
        }
        
        if (activity == 0) {
            // Timeout - no data received
            std::cout << "Timeout waiting for request from " << client_ip << std::endl;
            close(client_fd);
            return;
        }
        
        // Data is available, read the request
        char buffer[1024];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::string request(buffer);
            
            std::cout << "Received request from " << client_ip << ": " << request << std::endl;
            
            // Add request to pipeline
            addRequest(client_fd, client_ip, request);
        } else {
            // Error reading request
            std::cout << "Error reading request from " << client_ip << std::endl;
            close(client_fd);
        }
    }
    
private:
    // Stage 0: Request Handler - Parses requests and generates graphs
    void requestHandler(int stage_id) {
        std::cout << "Stage " << stage_id << " (Request Handler) started\n";
        
        while (running) {
            std::shared_ptr<PipelineData> data;
            
            {
                std::unique_lock<std::mutex> lock(request_mutex);
                request_cv.wait(lock, [this] { return !request_queue.empty() || !running; });
                
                if (!running) break;
                
                data = request_queue.front();
                request_queue.pop();
                std::cout << "Thread " << std::this_thread::get_id() << " picked up request from " 
                          << data->client_ip << " (queue size now: " << request_queue.size() << ")" << std::endl;
            }
            
            std::cout << "Stage " << stage_id << " (Thread " << std::this_thread::get_id() << ") processing request from " << data->client_ip << std::endl;
            
            try {
                // Parse request parameters - FAST parsing
                int edges = -1, vertices = -1, seed = -1;
                std::string algorithm = "EULER";
                
                // Use faster string parsing
                std::istringstream iss(data->request);
                std::string token;
                while (iss >> token) {
                    if (token == "-e" && iss >> edges) continue;
                    if (token == "-v" && iss >> vertices) continue;
                    if (token == "-s" && iss >> seed) continue;
                    if (token == "-a" && iss >> algorithm) {
                        algorithm = trim(algorithm); // Trim any whitespace/newlines
                        break;
                    }
                }
                
                if (edges < 0 || vertices <= 0) {
                    data->result = "ERROR: Invalid parameters";
                    // Send error response directly
                    if (send(data->client_fd, data->result.c_str(), data->result.length(), 0) < 0) {
                        perror("send");
                    }
                    close(data->client_fd);
                    completed_requests++;
                    continue;
                }
                
                // Generate graph - this is the heavy operation
                data->graph = std::make_unique<graph::Graph>(graph::Graph::generateRandomGraph(vertices, edges, seed));
                data->algorithm = algorithm;
                
                std::cout << "Stage " << stage_id << " generated graph with " << vertices << " vertices, " << edges << " edges\n";
                
                // TRUE PIPELINE: Every request goes through ALL algorithm stages
                std::cout << "Stage " << stage_id << " starting pipeline processing for " << data->client_ip << std::endl;
                
                // Add Euler circuit analysis to the result
                std::string euler_result = "";
                if (data->graph->hasEulerCircuit()) {
                    std::vector<int> circuit = data->graph->findEulerCircuit();
                    euler_result = "EULER CIRCUIT: SUCCESS!\n";
                    euler_result += "Circuit: ";
                    for (size_t i = 0; i < circuit.size(); ++i) {
                        if (i > 0) euler_result += " -> ";
                        euler_result += std::to_string(circuit[i]);
                    }
                    euler_result += "\n";
                } else {
                    euler_result = "EULER CIRCUIT: NOT POSSIBLE\n";
                    euler_result += "Reason: Graph is not connected or has odd-degree vertices\n";
                }
                
                // Store Euler result and send to MST processor (first stage)
                data->result = "GRAPH ANALYSIS RESULTS:\n";
                data->result += "Vertices: " + std::to_string(vertices) + "\n";
                data->result += "Edges: " + std::to_string(edges) + "\n";
                data->result += "Seed: " + std::to_string(seed) + "\n\n";
                data->result += euler_result + "\n";
                
                std::cout << "  → Sending to MST processor (queue size: " << mst_queue.size() << ")" << std::endl;
                {
                    std::lock_guard<std::mutex> lock(mst_mutex);
                    mst_queue.push(data);
                    mst_cv.notify_one();
                }
                
            } catch (const std::exception& e) {
                data->result = "ERROR: " + std::string(e.what());
                // Send error response directly
                if (send(data->client_fd, data->result.c_str(), data->result.length(), 0) < 0) {
                    perror("send");
                }
                close(data->client_fd);
                completed_requests++;
            }
        }
        
        std::cout << "Stage " << stage_id << " (Request Handler) finished\n";
    }
    
    // Stage 1: MST Weight Processor
    void mstProcessor(int stage_id) {
        std::cout << "Stage " << stage_id << " (MST Weight) started\n";
        
        while (running) {
            std::shared_ptr<PipelineData> data;
            
            {
                std::unique_lock<std::mutex> lock(mst_mutex);
                mst_cv.wait(lock, [this] { return !mst_queue.empty() || !running; });
                
                if (!running) break;
                
                data = mst_queue.front();
                mst_queue.pop();
            }
            
            std::cout << "Stage " << stage_id << " processing MST request from " << data->client_ip << std::endl;
            
            try {
                auto algo = graph::AlgorithmFactory::createAlgorithm(graph::AlgorithmFactory::AlgorithmType::MST_WEIGHT);
                if (algo) {
                    data->result += "=== MST WEIGHT ALGORITHM ===\n";
                    data->result += algo->getName() + "\n";
                    data->result += "Result: " + algo->execute(*data->graph) + "\n\n";
                    // Remove heavy analyzeGraph call to improve performance
                } else {
                    data->result += "ERROR: Failed to create MST algorithm instance\n\n";
                }
                
                // Send to SCC processor (next stage)
                std::cout << "Stage " << stage_id << " sending to SCC processor (queue size: " << scc_queue.size() << ")" << std::endl;
                {
                    std::lock_guard<std::mutex> lock(scc_mutex);
                    scc_queue.push(data);
                    scc_cv.notify_one();
                }
                
            } catch (const std::exception& e) {
                data->result += "ERROR: " + std::string(e.what()) + "\n\n";
                // Continue to next stage anyway
                {
                    std::lock_guard<std::mutex> lock(scc_mutex);
                    scc_queue.push(data);
                    scc_cv.notify_one();
                }
            }
        }
        
        std::cout << "Stage " << stage_id << " (MST Weight) finished\n";
    }
    
    // Stage 2: SCC Processor
    void sccProcessor(int stage_id) {
        std::cout << "Stage " << stage_id << " (SCC) started\n";
        
        while (running) {
            std::shared_ptr<PipelineData> data;
            
            {
                std::unique_lock<std::mutex> lock(scc_mutex);
                scc_cv.wait(lock, [this] { return !scc_queue.empty() || !running; });
                
                if (!running) break;
                
                data = scc_queue.front();
                scc_queue.pop();
            }
            
            std::cout << "Stage " << stage_id << " processing SCC request from " << data->client_ip << std::endl;
            
            try {
                auto algo = graph::AlgorithmFactory::createAlgorithm(graph::AlgorithmFactory::AlgorithmType::SCC);
                if (algo) {
                    data->result += "=== SCC ALGORITHM ===\n";
                    data->result += algo->getName() + "\n";
                    data->result += "Result: " + algo->execute(*data->graph) + "\n\n";
                    // Remove heavy analyzeGraph call to improve performance
                } else {
                    data->result += "ERROR: Failed to create SCC algorithm instance\n\n";
                }
                
                // Send to Max Flow processor (next stage)
                std::cout << "Stage " << stage_id << " sending to Max Flow processor (queue size: " << max_flow_queue.size() << ")" << std::endl;
                {
                    std::lock_guard<std::mutex> lock(max_flow_mutex);
                    max_flow_queue.push(data);
                    max_flow_cv.notify_one();
                }
                
            } catch (const std::exception& e) {
                data->result += "ERROR: " + std::string(e.what()) + "\n\n";
                // Continue to next stage anyway
                {
                    std::lock_guard<std::mutex> lock(max_flow_mutex);
                    max_flow_queue.push(data);
                    max_flow_cv.notify_one();
                }
            }
        }
        
        std::cout << "Stage " << stage_id << " (SCC) finished\n";
    }
    
    // Stage 3: Max Flow Processor
    void maxFlowProcessor(int stage_id) {
        std::cout << "Stage " << stage_id << " (Max Flow) started\n";
        
        while (running) {
            std::shared_ptr<PipelineData> data;
            
            {
                std::unique_lock<std::mutex> lock(max_flow_mutex);
                max_flow_cv.wait(lock, [this] { return !max_flow_queue.empty() || !running; });
                
                if (!running) break;
                
                data = max_flow_queue.front();
                max_flow_queue.pop();
            }
            
            std::cout << "Stage " << stage_id << " processing Max Flow request from " << data->client_ip << std::endl;
            
            try {
                auto algo = graph::AlgorithmFactory::createAlgorithm(graph::AlgorithmFactory::AlgorithmType::MAX_FLOW);
                if (algo) {
                    data->result += "=== MAX FLOW ALGORITHM ===\n";
                    data->result += algo->getName() + "\n";
                    data->result += "Result: " + algo->execute(*data->graph) + "\n\n";
                    // Remove heavy analyzeGraph call to improve performance
                } else {
                    data->result += "ERROR: Failed to create Max Flow algorithm instance\n\n";
                }
                
                // Send to Max Clique processor (next stage)
                std::cout << "Stage " << stage_id << " sending to Max Clique processor (queue size: " << max_clique_queue.size() << ")" << std::endl;
                {
                    std::lock_guard<std::mutex> lock(max_clique_mutex);
                    max_clique_queue.push(data);
                    max_clique_cv.notify_one();
                }
                
            } catch (const std::exception& e) {
                data->result += "ERROR: " + std::string(e.what()) + "\n\n";
                // Continue to next stage anyway
                {
                    std::lock_guard<std::mutex> lock(max_clique_mutex);
                    max_clique_queue.push(data);
                    max_clique_cv.notify_one();
                }
            }
        }
        
        std::cout << "Stage " << stage_id << " (Max Flow) finished\n";
    }
    
    // Stage 4: Max Clique Processor
    void maxCliqueProcessor(int stage_id) {
        std::cout << "Stage " << stage_id << " (Max Clique) started\n";
        
        while (running) {
            std::shared_ptr<PipelineData> data;
            
            {
                std::unique_lock<std::mutex> lock(max_clique_mutex);
                max_clique_cv.wait(lock, [this] { return !max_clique_queue.empty() || !running; });
                
                if (!running) break;
                
                data = max_clique_queue.front();
                max_clique_queue.pop();
            }
            
            std::cout << "Stage " << stage_id << " processing Max Clique request from " << data->client_ip << std::endl;
            
            try {
                auto algo = graph::AlgorithmFactory::createAlgorithm(graph::AlgorithmFactory::AlgorithmType::MAX_CLIQUE);
                if (algo) {
                    data->result += "=== MAX CLIQUE ALGORITHM ===\n";
                    data->result += algo->getName() + "\n";
                    data->result += "Result: " + algo->execute(*data->graph) + "\n\n";
                    // Remove heavy analyzeGraph call to improve performance
                } else {
                    data->result += "ERROR: Failed to create Max Clique algorithm instance\n\n";
                }
                
                // Send to response stage (final stage)
                std::cout << "Stage " << stage_id << " sending to response stage (queue size: " << response_queue.size() << ")" << std::endl;
                {
                    std::lock_guard<std::mutex> lock(response_mutex);
                    response_queue.push(data);
                    response_cv.notify_one();
                }
                
            } catch (const std::exception& e) {
                data->result += "ERROR: " + std::string(e.what()) + "\n\n";
                // Send to response stage anyway
                {
                    std::lock_guard<std::mutex> lock(response_mutex);
                    response_queue.push(data);
                    response_cv.notify_one();
                }
            }
        }
        
        std::cout << "Stage " << stage_id << " (Max Clique) finished\n";
    }
    
    // Stage 5: Response Sender
    void responseSender(int stage_id) {
        std::cout << "Stage " << stage_id << " (Response Sender) started\n";
        
        while (running) {
            std::shared_ptr<PipelineData> data;
            
            {
                std::unique_lock<std::mutex> lock(response_mutex);
                response_cv.wait(lock, [this] { return !response_queue.empty() || !running; });
                
                if (!running) break;
                
                data = response_queue.front();
                response_queue.pop();
            }
            
            std::cout << "Stage " << stage_id << " sending response to " << data->client_ip << std::endl;
            
            // Calculate processing time
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - data->start_time);
            
            // Add timing information to response
            std::string full_response = data->result + "\n\nPipeline processing time: " + std::to_string(duration.count()) + " microseconds\n";
            
            // Send response to client
            if (send(data->client_fd, full_response.c_str(), full_response.length(), 0) < 0) {
                perror("send");
            }
            
            // Close connection
            close(data->client_fd);
            completed_requests++;
            
            std::cout << "Stage " << stage_id << " completed response for " << data->client_ip 
                      << " in " << duration.count() << " microseconds\n";
        }
        
        std::cout << "Stage " << stage_id << " (Response Sender) finished\n";
    }
};

// Global server instance
std::unique_ptr<PipelineServer> pipeline_server;

// Signal handler for graceful shutdown
void signal_handler(int sig)
{
    running = 0;
    std::cout << "\nReceived signal " << sig << ", shutting down server" << std::endl;
    
    // Shutdown pipeline server
    if (pipeline_server) {
        pipeline_server->shutdown();
    }

    if (listen_socket_fd != -1)
    {
        close(listen_socket_fd);
        listen_socket_fd = -1;
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);

    // TCP Server setup
    int tcp_port;

    if (argc != 2)
    {
        std::cerr << "Error: Number of parameters is incorrect\n";
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    else
    {
        tcp_port = atoi(argv[1]);
    }

    if (tcp_port <= 0 || tcp_port > 65535)
    {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }

    // Create socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        return 1;
    }

    listen_socket_fd = listen_fd;

    // Set socket options
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, BACKLOG) < 0)
    {
        perror("listen");
        close(listen_fd);
        return 1;
    }

    std::cout << "Pipeline Server listening on port " << tcp_port << std::endl;
    std::cout << "Pipeline stages: " << PIPELINE_STAGES << std::endl;
    std::cout << "Waiting for connections..." << std::endl;

    // Initialize Pipeline server
    pipeline_server = std::make_unique<PipelineServer>();

    // Main accept loop
    while (running)
    {
        // Use select to check for incoming connections with timeout
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms timeout

        int activity = select(listen_fd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0)
        {
            if (running) perror("select");
            continue;
        }

        if (!running)
        {
            break;
        }

        // Check if there's a new connection ready
        if (activity == 0 || !FD_ISSET(listen_fd, &readfds))
        {
            continue;
        }

        // Accept new connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "New connection from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Add connection to pipeline without waiting for request
        // The pipeline will handle reading the request when ready
        pipeline_server->addConnection(client_fd, client_ip);
        
        // Print statistics periodically
        static int connection_count = 0;
        if (++connection_count % 10 == 0) {
            pipeline_server->printStats();
        }
    }

    close(listen_fd);
    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}
