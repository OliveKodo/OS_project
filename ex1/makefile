# ==== Compiler & flags ====
CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic -std=c++17 -g -O0

# gcov tool adaptation for compiler
ifeq ($(findstring clang++,$(CXX)),clang++)
	GCOV ?= llvm-cov gcov
else
	GCOV ?= gcov
endif

# ==== Project files ====
SOURCES := main.cpp graph.cpp point.cpp
HEADERS := graph.hpp point.hpp
TARGET := graph

# ==== Derived ====
OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS_COV := $(SOURCES:.cpp=.cov.o)
OBJECTS_PG := $(SOURCES:.cpp=.pg.o)

# ================== default build ==================
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ================== coverage (gcov) ==================
coverage: clean
	@chmod +x coverage_test.sh
	@./coverage_test.sh full
	@rm -rf build 

# ================== gprof ==================
gprof: clean-reports $(TARGET)_pg
	@echo ">> Running program for gprof analysis..."
	-./$(TARGET)_pg -t 2>/dev/null || true
	-./$(TARGET)_pg -v 6 -e 8 -s 123 2>/dev/null || true
	@if [ -f gmon.out ]; then \
		gprof $(TARGET)_pg gmon.out > gprof_report.txt; \
		echo "gprof report -> gprof_report.txt"; \
	else \
		echo "Warning: gmon.out not found. Make sure the program runs successfully."; \
	fi

$(TARGET)_pg: $(OBJECTS_PG)
	$(CXX) $(CXXFLAGS) -pg -o $@ $^

%.pg.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -pg -c $< -o $@

# ================== valgrind ==================
valgrind: $(TARGET)
	@echo ">> Running Valgrind memcheck..."
	@$(MAKE) --quiet $(TARGET)   # (re)build if coverage removed it
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--log-file=valgrind_memcheck.log \
		./$(TARGET) -t
	@echo "Valgrind memcheck log -> valgrind_memcheck.log"
# ================== everything for Part 4 ==================
part4: all coverage gprof valgrind callgrind
	@echo "=== Part 4 artifacts ==="
	@ls -1 coverage.txt *.gcov gprof_report.txt valgrind_memcheck.log callgrind.out 2>/dev/null || true

# ================== cleaning ==================
clean:
	rm -f $(TARGET) $(TARGET)_cov $(TARGET)_pg
	rm -f $(OBJECTS) $(OBJECTS_COV) $(OBJECTS_PG)
	rm -f *.log *.gcov *.gcda *.gcno *.out
	rm -f coverage.txt gprof_report.txt
	rm -f core core.* gmon.out callgrind.out

clean-reports:
	rm -f *.log *.gcov *.gcda *.gcno *.out
	rm -f coverage.txt gprof_report.txt
	rm -f core core.* gmon.out callgrind.out

.PHONY: all coverage gprof valgrind callgrind part4 clean clean-reports