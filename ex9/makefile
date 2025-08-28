CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic -std=c++17 -g -O0

SERVER_TARGET = tcp_server
CLIENT_TARGET = tcp_client
TEST_TARGET = test_algorithms

SERVER_SOURCES = tcp_server.cpp graph.cpp point.cpp graph_algorithms.cpp
CLIENT_SOURCES = client.cpp
TEST_SOURCES = test_algorithms.cpp graph.cpp point.cpp graph_algorithms.cpp

TARGETS = $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

all: $(TARGETS)

$(SERVER_TARGET): $(SERVER_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@ -pthread

$(CLIENT_TARGET): $(CLIENT_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

$(TEST_TARGET): $(TEST_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Valgrind flags
VG_FLAGS ?= --leak-check=full --show-leak-kinds=all --track-origins=yes

# Section 10 - Complete Valgrind analysis for Pipeline pattern
valgrind: valgrind-memcheck valgrind-helgrind valgrind-cachegrind
	@echo "=== SECTION 10 VALGRIND ANALYSIS COMPLETE ==="
	@echo "Generated logs:"
	@echo "  - valgrind.memcheck.log (memory analysis)"
	@echo "  - valgrind.helgrind.log (thread analysis)" 
	@echo "  - valgrind.cachegrind.log (cache analysis)"

valgrind-memcheck: $(SERVER_TARGET) $(CLIENT_TARGET)
	@valgrind --tool=memcheck $(VG_FLAGS) --log-file=valgrind.memcheck.log ./$(SERVER_TARGET) 8080 & srv=$$!; \
	sleep 2; \
	(echo "-e 8 -v 6 -s 42 -a MST_WEIGHT" | ./$(CLIENT_TARGET) 127.0.0.1 8080) || true; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then kill -INT $$srv; wait $$srv 2>/dev/null || true; fi

valgrind-helgrind: $(SERVER_TARGET) $(CLIENT_TARGET)
	@valgrind --tool=helgrind --log-file=valgrind.helgrind.log ./$(SERVER_TARGET) 8080 & srv=$$!; \
	sleep 2; \
	(echo "-e 8 -v 6 -s 42 -a MST_WEIGHT" | ./$(CLIENT_TARGET) 127.0.0.1 8080) || true; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then kill -INT $$srv; wait $$srv 2>/dev/null || true; fi

valgrind-cachegrind: $(SERVER_TARGET) $(CLIENT_TARGET)
	@valgrind --tool=cachegrind --log-file=valgrind.cachegrind.log ./$(SERVER_TARGET) 8080 & srv=$$!; \
	sleep 2; \
	(echo "-e 8 -v 6 -s 42 -a MST_WEIGHT" | ./$(CLIENT_TARGET) 127.0.0.1 8080) || true; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then kill -INT $$srv; wait $$srv 2>/dev/null || true; fi

valgrind-test: $(TEST_TARGET)
	valgrind $(VG_FLAGS) --log-file=valgrind.test.log ./$(TEST_TARGET)

valgrind-server: $(SERVER_TARGET)
	valgrind $(VG_FLAGS) --log-file=valgrind.server.log ./$(SERVER_TARGET) 8080

clean:
	rm -f *.o $(TARGETS) *.log

server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)  
test: $(TEST_TARGET)

.PHONY: all clean server client test valgrind valgrind-memcheck valgrind-helgrind valgrind-cachegrind valgrind-test valgrind-server