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
#include "graph.hpp"
#include "graph_algorithm.hpp"

#define BACKLOG 5

// main loop global var
volatile sig_atomic_t running = 1;

// vector with all actiive connections
std::vector<int> active_connections;
std::mutex connections_mutex;
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

std::string processGraphRequest(const std::string &request)
{
    // Parse request like "-e 5 -v 4 -s 43 -a MST_WEIGHT"
    int edges = -1, vertices = -1, seed = -1;
    std::string algorithm = "EULER"; // Default to Euler circuit

    // Parse parameters
    size_t pos = request.find("-e ");
    if (pos != std::string::npos)
    {
        edges = std::stoi(request.substr(pos + 3));
    }

    pos = request.find("-v ");
    if (pos != std::string::npos)
    {
        vertices = std::stoi(request.substr(pos + 3));
    }

    pos = request.find("-s ");
    if (pos != std::string::npos)
    {
        seed = std::stoi(request.substr(pos + 3));
    }

    pos = request.find("-a ");
    if (pos != std::string::npos)
    {
        algorithm = request.substr(pos + 3);
    }

    if (edges < 0 || vertices <= 0)
    {
        return "ERROR: Invalid parameters. Use format: -e <edges> -v <vertices> -s <seed> [-a <algorithm>]\n"
               "Available algorithms: EULER, MST_WEIGHT, SCC, MAX_FLOW, MAX_CLIQUE";
    }

    try
    {
        // Generate random graph
        graph::Graph graph = graph::Graph::generateRandomGraph(vertices, edges, seed);
        
        std::string result;
        
        if (algorithm == "EULER" || algorithm == "EULER_CIRCUIT") {
            // Original Euler circuit logic
            if (graph.hasEulerCircuit())
            {
                std::vector<int> circuit = graph.findEulerCircuit();
                result = "SUCCESS: Graph has Euler circuit!\n";
                result += "Vertices: " + std::to_string(vertices) + "\n";
                result += "Edges: " + std::to_string(edges) + "\n";
                result += "Circuit: ";
                for (size_t i = 0; i < circuit.size(); ++i)
                {
                    if (i > 0)
                        result += " -> ";
                    result += std::to_string(circuit[i]);
                }
                analyzeGraph(graph);
            }
            else
            {
                result = "RESULT: Graph does NOT have Euler circuit\n";
                result += "Vertices: " + std::to_string(vertices) + "\n";
                result += "Edges: " + std::to_string(edges) + "\n";
                result += "Reason: Graph is not connected or has odd-degree vertices";
                analyzeGraph(graph);
            }
        } else {
            graph::AlgorithmFactory::AlgorithmType algoType;
            
            if (algorithm == "MST_WEIGHT") {
                algoType = graph::AlgorithmFactory::AlgorithmType::MST_WEIGHT;
            } else if (algorithm == "SCC") {
                algoType = graph::AlgorithmFactory::AlgorithmType::SCC;
            } else if (algorithm == "MAX_FLOW") {
                algoType = graph::AlgorithmFactory::AlgorithmType::MAX_FLOW;
            } else if (algorithm == "MAX_CLIQUE") {
                algoType = graph::AlgorithmFactory::AlgorithmType::MAX_CLIQUE;
            } else {
                return "ERROR: Unknown algorithm '" + algorithm + "'. Available: EULER, MST_WEIGHT, SCC, MAX_FLOW, MAX_CLIQUE";
            }
            
            auto algo = graph::AlgorithmFactory::createAlgorithm(algoType);
            if (!algo) {
                return "ERROR: Failed to create algorithm instance";
            }
            
            result = "ALGORITHM: " + algo->getName() + "\n";
            result += "Vertices: " + std::to_string(vertices) + "\n";
            result += "Edges: " + std::to_string(edges) + "\n";
            result += "Seed: " + std::to_string(seed) + "\n";
            result += "Result:\n";
            result += algo->execute(graph);

            
            // Display graph info
            analyzeGraph(graph);
        }
        
        return result;
    }
    catch (const std::exception &e)
    {
        return "ERROR: " + std::string(e.what());
    }
}

void handleClient(int client_fd, const std::string &client_ip)
{
    std::cout << "Starting client handler for " << client_ip << std::endl;

    while (running)
    {
        char buffer[1024];
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            std::string request(buffer);
            std::cout << "Received from " << client_ip << ": " << request << std::endl;

            // proccess request
            std::string response = processGraphRequest(request);

             if (send(client_fd, response.c_str(), response.length(), 0) < 0)
            {
                perror("send");
                break;
            }
        }
        else if (bytes_received == 0)
        {
            std::cout << "Client " << client_ip << " disconnected" << std::endl;
            break;
        }
        else
        {
            perror("recv");
            break;
        }
    }


    {
        std::lock_guard<std::mutex> lock(connections_mutex);
        auto it = std::find(active_connections.begin(), active_connections.end(), client_fd);
        if (it != active_connections.end())
        {
            active_connections.erase(it);
        }
    }

    close(client_fd);
    std::cout << "Connection with " << client_ip << " closed" << std::endl;
}

// global fd for closing connections
int listen_socket_fd = -1;

// sigint
void signal_handler(int sig)
{
    running = 0;
    std::cout << "\nReceived signal " << sig << ", shutting down server" << std::endl;

    // sending to clients that the server is closed
    {
        std::lock_guard<std::mutex> lock(connections_mutex);
        for (int client_fd : active_connections)
        {
            std::string bye_msg = "SERVER_SHUTDOWN";
            send(client_fd, bye_msg.c_str(), bye_msg.length(), 0);
            close(client_fd);
        }
        active_connections.clear();
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

    // ------ TCP SERVER SETTING --------
    int tcp_port;

    if (argc != 2)
    {
        std::cerr << "Error: Number of paramerters is incorrect\n";
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

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        return 1;
    }

    listen_socket_fd = listen_fd;

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

    std::cout << "Server listening on port " << tcp_port << std::endl;
    std::cout << "Waiting for connections..." << std::endl;
    // -------------------------------------------

    // --------  MAIN LOOP --------
    std::vector<std::thread> client_threads;

    while (running)
    {
        // add lesten_fd to fd_set WILL BE OUR NEW CONNECTION FILE DESCRIPTOR
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        // checks if there is a new connection
        int activity = select(listen_fd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0)
        {
            perror("select");
            continue;
        }

        if (!running)
        {
            break;
        }

        // verifies if listen_fd is ready to accept new clients
        if (activity == 0 || !FD_ISSET(listen_fd, &readfds))
        {
            continue;
        }

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

        // add new clients to active connections
        {
            std::lock_guard<std::mutex> lock(connections_mutex);
            active_connections.push_back(client_fd);
        }

        // threrad for new client
        std::thread client_thread(handleClient, client_fd, client_ip);
        client_threads.push_back(std::move(client_thread));

        std::cout << "Client handler started for " << client_ip << std::endl;
    }

    std::cout << "Waiting for all client threads to finish..." << std::endl;
    for (auto &thread : client_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    close(listen_fd);
    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}