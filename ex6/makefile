CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic -std=c++17 -g -O0

SERVER_TARGET = tcp_server
CLIENT_TARGET = tcp_client
SERVER_SOURCES = tcp_server.cpp graph.cpp point.cpp
CLIENT_SOURCES = client.cpp

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@ -pthread

$(CLIENT_TARGET): $(CLIENT_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

valgrind: $(SERVER_TARGET) $(CLIENT_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(SERVER_TARGET) & \
	valgrind --leak-check=full --show-leak-kinds=all ./$(CLIENT_TARGET)

clean:
	rm -f *.o $(SERVER_TARGET) $(CLIENT_TARGET)

server: $(SERVER_TARGET)

client: $(CLIENT_TARGET)
