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

clean:
	rm -f *.o $(TARGETS) *.log *.gcov *.gcda *.gcno

# ---------- Full Coverage Testing ----------
# Coverage flags
COVERAGE_CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage

# Full coverage test including server functionality
coverage: coverage-clean coverage-build coverage-run coverage-report

coverage-clean:
	@echo "Cleaning previous build and coverage files..."
	rm -f *.o $(TARGETS) *.gcov *.gcda *.gcno

coverage-build:
	@echo "Building with coverage flags..."
	g++ $(COVERAGE_CXXFLAGS) -c graph.cpp -o graph.o
	g++ $(COVERAGE_CXXFLAGS) -c point.cpp -o point.o
	g++ $(COVERAGE_CXXFLAGS) -c graph_algorithms.cpp -o graph_algorithms.o
	g++ $(COVERAGE_CXXFLAGS) -c test_algorithms.cpp -o test_algorithms.o
	g++ $(COVERAGE_CXXFLAGS) -c tcp_server.cpp -o tcp_server.o
	g++ $(COVERAGE_CXXFLAGS) -c client.cpp -o client.o
	g++ $(COVERAGE_CXXFLAGS) graph.o point.o graph_algorithms.o test_algorithms.o -o test_algorithms
	g++ $(COVERAGE_CXXFLAGS) tcp_server.o graph.o point.o graph_algorithms.o -o tcp_server -pthread
	g++ $(COVERAGE_CXXFLAGS) client.o -o tcp_client

coverage-run:
	@echo "Running algorithm tests to generate coverage data..."
	./test_algorithms
	@echo "Testing server functionality for coverage..."
	@echo "Starting server on port 8080..."
	@./tcp_server 8080 & srv=$$!; \
	sleep 3; \
	echo "Sending test requests to generate server coverage..."; \
	echo "-e 6 -v 5 -s 15 -a MST_WEIGHT" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 8 -v 6 -s 20 -a SCC" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 10 -v 7 -s 25 -a MAX_FLOW" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 12 -v 8 -s 30 -a MAX_CLIQUE" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 4 -v 3 -s 10" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 15 -v 10 -s 50 -a MST_WEIGHT" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "Testing invalid requests for error handling coverage..."; \
	echo "invalid_request" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e -5 -v 3 -s 10" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 5 -v 10 -s 10" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "Testing edge cases for better coverage..."; \
	echo "-e 1 -v 2 -s 1" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 20 -v 5 -s 100" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 0 -v 3 -s 5" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 5 -v 0 -s 5" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 5 -v 3 -s -5" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 5 -v 3 -s 5 -a INVALID_ALGO" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 5 -v 3 -s 5 -a" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "-e 5 -v 3 -s 5 -a MST_WEIGHT -extra_param" | ./tcp_client 127.0.0.1 8080; \
	sleep 1; \
	echo "Testing concurrent requests for thread coverage..."; \
	echo "-e 6 -v 4 -s 10 -a MST_WEIGHT" | ./tcp_client 127.0.0.1 8080 & \
	echo "-e 7 -v 5 -s 15 -a SCC" | ./tcp_client 127.0.0.1 8080 & \
	echo "-e 8 -v 6 -s 20 -a MAX_FLOW" | ./tcp_client 127.0.0.1 8080 & \
	sleep 3; \
	echo "Stopping server..."; \
	kill -INT $$srv 2>/dev/null || true; \
	wait $$srv 2>/dev/null || true; \
	echo "Server tests completed!"

coverage-report:
	@echo "Generating coverage reports for YOUR source files only..."
	@echo "========================================"
	@for src_file in graph.cpp point.cpp graph_algorithms.cpp tcp_server.cpp client.cpp; do \
		if [ -f "$$src_file" ]; then \
			echo "Processing coverage for $$src_file "; \
			gcov -b -c "$$src_file" >/dev/null 2>&1; \
			if [ -f "$${src_file}.gcov" ]; then \
				echo "Coverage for $$src_file:"; \
				echo "----------------------------------------"; \
				total_lines=0; \
				covered_lines=0; \
				while IFS=: read -r count _ rest; do \
					count="$${count#"$${count%%[![:space:]]*}"}"; \
					if [ -n "$$count" ] && [ "$$count" != "-" ]; then \
						if [ "$$count" = "#####" ]; then \
							total_lines=$$((total_lines + 1)); \
						elif echo "$$count" | grep -q '^[0-9]\+$$'; then \
							total_lines=$$((total_lines + 1)); \
							if [ "$$count" -gt 0 ]; then \
								covered_lines=$$((covered_lines + 1)); \
							fi; \
						fi; \
					fi; \
				done < "$${src_file}.gcov"; \
				if [ "$$total_lines" -gt 0 ]; then \
					pct=$$(( (covered_lines*100 + total_lines/2) / total_lines )); \
					echo "Lines executed: $$pct% ($$covered_lines/$$total_lines)"; \
					echo "File: $${src_file}.gcov created"; \
				else \
					echo "No executable lines found"; \
				fi; \
				echo; \
			else \
				echo "No coverage data generated for $$src_file"; \
			fi; \
		fi; \
	done
	@echo "========================================"
	@echo "FULL COVERAGE TEST COMPLETE"
	@echo "========================================"
	@echo "Coverage files created for YOUR source files only:"
	@for src_file in graph.cpp point.cpp graph_algorithms.cpp tcp_server.cpp client.cpp; do \
		if [ -f "$${src_file}.gcov" ]; then \
			echo "  ✓ $${src_file}.gcov"; \
		fi; \
	done
	@echo
	@echo "External library coverage files have been filtered out."
	@echo "Only YOUR source code coverage is shown above."
	@echo
	@echo "Coverage summary:"
	@echo "  - graph.cpp.gcov - Graph implementation coverage"
	@echo "  - point.cpp.gcov - Point class coverage"
	@echo "  - graph_algorithms.cpp.gcov - Algorithm implementations coverage"
	@echo "  - tcp_server.cpp.gcov - Server implementation coverage"
	@echo "  - client.cpp.gcov - Client implementation coverage"

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

server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)  
test: $(TEST_TARGET)

.PHONY: all clean server client test valgrind valgrind-memcheck valgrind-helgrind valgrind-cachegrind valgrind-test valgrind-server coverage coverage-clean coverage-build coverage-run coverage-report