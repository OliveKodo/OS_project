#!/bin/bash

echo "========================================"
echo "EX8 COVERAGE TEST FOR UBUNTU"
echo "========================================"
echo

# Check if gcov is available
if ! command -v gcov &> /dev/null; then
    echo "ERROR: gcov not found. Please install gcov:"
    echo "sudo apt-get install gcov"
    exit 1
fi

echo "Cleaning previous build..."
make clean

echo "Building with coverage flags..."
# Build with coverage flags - we need to compile AND link with coverage
CXXFLAGS="-std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage" make clean

# Compile individual object files with coverage
echo "Compiling source files with coverage..."
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage -c graph.cpp -o graph.o
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage -c point.cpp -o point.o
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage -c graph_algorithms.cpp -o graph_algorithms.o
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage -c test_algorithms.cpp -o test_algorithms.o

# Link with coverage library
echo "Linking test executable with coverage..."
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O0 --coverage graph.o point.o graph_algorithms.o test_algorithms.o -o test_algorithms

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi
echo "Build successful with coverage enabled!"
echo

echo "Running algorithm tests to generate coverage data..."
./test_algorithms
if [ $? -ne 0 ]; then
    echo "ERROR: Algorithm tests failed!"
    exit 1
fi
echo "Algorithm tests completed!"
echo

echo "Generating coverage reports for YOUR source files only..."
echo "========================================"

# Only process YOUR source files, not external libraries
YOUR_SOURCE_FILES=("graph.cpp" "point.cpp" "graph_algorithms.cpp")

for src_file in "${YOUR_SOURCE_FILES[@]}"; do
    if [ -f "$src_file" ]; then
        echo "Processing coverage for $src_file (YOUR file)..."
        
        # Generate .gcov file
        gcov -b -c "$src_file" 2>/dev/null
        
        # Show coverage summary
        if [ -f "${src_file}.gcov" ]; then
            echo "Coverage for $src_file:"
            echo "----------------------------------------"
            
            # Count lines and coverage
            total_lines=0
            covered_lines=0
            while IFS=: read -r count _ rest; do
                count="${count#"${count%%[![:space:]]*}"}"  # trim whitespace
                if [[ -n "$count" && "$count" != "-" ]]; then
                    if [[ "$count" == "#####" ]]; then
                        ((total_lines++))
                    elif [[ "$count" =~ ^[0-9]+$ ]]; then
                        ((total_lines++))
                        (( count > 0 )) && ((covered_lines++))
                    fi
                fi
            done < "${src_file}.gcov"
            
            if (( total_lines > 0 )); then
                pct=$(( (covered_lines*100 + total_lines/2) / total_lines ))
                echo "Lines executed: $pct% ($covered_lines/$total_lines)"
                echo "File: ${src_file}.gcov created"
            else
                echo "No executable lines found"
            fi
            echo
        else
            echo "No coverage data generated for $src_file"
        fi
    fi
done

echo "========================================"
echo "COVERAGE TEST COMPLETE"
echo "========================================"
echo
echo "Coverage files created for YOUR source files only:"
for src_file in "${YOUR_SOURCE_FILES[@]}"; do
    if [ -f "${src_file}.gcov" ]; then
        echo "  ✓ ${src_file}.gcov"
    fi
done
echo
echo "External library coverage files have been filtered out."
echo "Only YOUR source code coverage is shown above."
echo
echo "To view detailed coverage:"
echo "  - Open any .gcov file in a text editor"
echo "  - Lines starting with '#####' are not executed"
echo "  - Lines starting with numbers show execution count"
echo
echo "Coverage summary:"
echo "  - graph.cpp.gcov - Graph implementation coverage"
echo "  - point.cpp.gcov - Point class coverage"
echo "  - graph_algorithms.cpp.gcov - Algorithm implementations coverage"
echo
