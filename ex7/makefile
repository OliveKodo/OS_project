CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic -std=c++17 -g -O0

SERVER_TARGET = tcp_server
CLIENT_TARGET = tcp_client
TEST_TARGET = test_algorithms
SERVER_SOURCES = tcp_server.cpp graph.cpp point.cpp graph_algorithms.cpp
CLIENT_SOURCES = client.cpp
TEST_SOURCES = test_algorithms.cpp graph.cpp point.cpp graph_algorithms.cpp

TARGET = $(SERVER_TARGET) $(CLIENT_TARGET) $(SERVER_SOURCES) $(CLIENT_SOURCES) $(TEST_TARGET) 

all: $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

$(SERVER_TARGET): $(SERVER_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@ -pthread

$(CLIENT_TARGET): $(CLIENT_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

$(TEST_TARGET): $(TEST_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f *.o $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

server: $(SERVER_TARGET)

client: $(CLIENT_TARGET)

test: $(TEST_TARGET)
