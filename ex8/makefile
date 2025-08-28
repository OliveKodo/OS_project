# ---------- Build Configuration ----------
SHELL := /bin/bash
CXX      ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic -std=c++17 -g -O0

# Target definitions
SERVER_TARGET := lf_server
CLIENT_TARGET := tcp_client
TEST_TARGET   := test_algorithms
BINARIES      := $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

# Source file definitions
SERVER_SOURCES := lf_server.cpp graph.cpp point.cpp graph_algorithms.cpp
CLIENT_SOURCES := client.cpp
TEST_SOURCES   := test_algorithms.cpp graph.cpp point.cpp graph_algorithms.cpp

# ---------- Build Rules ----------
all: $(BINARIES)

$(SERVER_TARGET): $(SERVER_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@ -pthread

$(CLIENT_TARGET): $(CLIENT_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

$(TEST_TARGET): $(TEST_SOURCES:.cpp=.o)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(BINARIES) valgrind.*.log *.log

# Convenience targets
server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)
test:   $(TEST_TARGET)

# ---------- Valgrind Configuration ----------
VG_MEMCHECK_FLAGS ?= --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
VG_HELGRIND_FLAGS ?= --tool=helgrind --verbose
VG_CACHEGRIND_FLAGS ?= --tool=cachegrind --verbose

# Test configuration
HOST     ?= 127.0.0.1
PORT     ?= 8080
REQUEST  ?= -e 8 -v 6 -s 42 -a MST_WEIGHT

# ---------- Valgrind Analysis ----------
# Memory check analysis
valgrind-memcheck: $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Starting memcheck analysis on $(HOST):$(PORT)"
	@valgrind $(VG_MEMCHECK_FLAGS) --log-file=valgrind.memcheck.log ./$(SERVER_TARGET) $(PORT) & srv=$$!; \
	sleep 2; \
	echo "Sending test request: $(REQUEST)"; \
	(echo "$(REQUEST)" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) || true; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then \
		echo "Terminating server"; \
		kill -INT $$srv; \
		wait $$srv 2>/dev/null || true; \
	else \
		echo "Server already terminated"; \
	fi; \
	echo "Memcheck analysis complete. Log: valgrind.memcheck.log"

# Thread analysis for race conditions and deadlocks
valgrind-helgrind: $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Starting helgrind analysis on $(HOST):$(PORT)"
	@valgrind $(VG_HELGRIND_FLAGS) --log-file=valgrind.helgrind.log ./$(SERVER_TARGET) $(PORT) & srv=$$!; \
	sleep 2; \
	echo "Sending test request: $(REQUEST)"; \
	(echo "$(REQUEST)" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) || true; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then \
		echo "Terminating server"; \
		kill -INT $$srv; \
		wait $$srv 2>/dev/null || true; \
	else \
		echo "Server already terminated"; \
	fi; \
	echo "Helgrind analysis complete. Log: valgrind.helgrind.log"

# Cache performance analysis
valgrind-cachegrind: $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Starting cachegrind analysis on $(HOST):$(PORT)"
	@valgrind $(VG_CACHEGRIND_FLAGS) --log-file=valgrind.cachegrind.log ./$(SERVER_TARGET) $(PORT) & srv=$$!; \
	sleep 2; \
	echo "Sending test request: $(REQUEST)"; \
	(echo "$(REQUEST)" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) || true; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then \
		echo "Terminating server"; \
		kill -INT $$srv; \
		wait $$srv 2>/dev/null || true; \
	else \
		echo "Server already terminated"; \
	fi; \
	echo "Cachegrind analysis complete. Log: valgrind.cachegrind.log"

# Complete section 10 analysis - all three required tools
valgrind-section10: valgrind-memcheck valgrind-helgrind valgrind-cachegrind
	@echo "=== SECTION 10 VALGRIND ANALYSIS COMPLETE ==="
	@echo "Generated logs:"
	@echo "  - valgrind.memcheck.log (memory analysis)"
	@echo "  - valgrind.helgrind.log (thread analysis)" 
	@echo "  - valgrind.cachegrind.log (cache analysis)"
	@echo "=== ALL THREE ANALYSES REQUIRED FOR SECTION 10 ==="

# Run all valgrind analyses (alias)
valgrind-all: valgrind-section10

# Default valgrind target - now covers complete section 10
valgrind: valgrind-section10

# ---------- Manual Testing ----------
# Run server under valgrind manually (for interactive testing)
valgrind-server: $(SERVER_TARGET)
	valgrind $(VG_MEMCHECK_FLAGS) ./$(SERVER_TARGET) $(PORT)

# Run test suite under valgrind
valgrind-test: $(TEST_TARGET)
	valgrind $(VG_MEMCHECK_FLAGS) --log-file=valgrind.test.log ./$(TEST_TARGET)
	@echo "Test analysis complete. Log: valgrind.test.log"

# Send request to running server
request: $(CLIENT_TARGET)
	@echo "Sending request: $(REQUEST) to $(HOST):$(PORT)"
	@(echo "$(REQUEST)" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) || echo "Connection failed"

# ---------- Multi-client Testing ----------
# Test with multiple concurrent clients
test-concurrent: $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Starting server for concurrent test"
	@./$(SERVER_TARGET) $(PORT) & srv=$$!; \
	sleep 2; \
	echo "Sending 3 concurrent requests"; \
	(echo "-e 5 -v 4 -s 10" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) & \
	(echo "-e 8 -v 6 -s 20 -a MST_WEIGHT" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) & \
	(echo "-e 12 -v 8 -s 30 -a SCC" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) & \
	wait; \
	sleep 1; \
	if kill -0 $$srv 2>/dev/null; then \
		kill -INT $$srv; \
		wait $$srv 2>/dev/null || true; \
	fi; \
	echo "Concurrent test complete"

# Test concurrent clients under valgrind
valgrind-concurrent: $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo "Starting concurrent test under valgrind"
	@valgrind $(VG_MEMCHECK_FLAGS) --log-file=valgrind.concurrent.log ./$(SERVER_TARGET) $(PORT) & srv=$$!; \
	sleep 3; \
	echo "Sending 3 concurrent requests under analysis"; \
	(echo "-e 5 -v 4 -s 10" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) & \
	(echo "-e 8 -v 6 -s 20 -a MST_WEIGHT" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) & \
	(echo "-e 12 -v 8 -s 30 -a SCC" | ./$(CLIENT_TARGET) $(HOST) $(PORT)) & \
	wait; \
	sleep 2; \
	if kill -0 $$srv 2>/dev/null; then \
		kill -INT $$srv; \
		wait $$srv 2>/dev/null || true; \
	fi; \
	echo "Concurrent valgrind analysis complete. Log: valgrind.concurrent.log"

# ---------- Phony Targets ----------
.PHONY: all clean server client test \
        valgrind valgrind-memcheck valgrind-helgrind valgrind-cachegrind valgrind-all \
        valgrind-server valgrind-test valgrind-concurrent \
        request test-concurrent