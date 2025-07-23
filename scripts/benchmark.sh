#!/bin/bash

# CLog Performance Benchmark Script
# Measures logging performance across different scenarios

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}=== CLog Performance Benchmark ===${NC}"

# Build if needed
if [ ! -d "build" ] || [ ! -f "build/tests/integration/test_embedded_simulation" ]; then
    echo "Building benchmark tests..."
    mkdir -p build
    cd build
    cmake -DCLOG_BUILD_TESTS=ON .. > /dev/null
    make > /dev/null
    cd ..
fi

echo -e "${YELLOW}Running performance benchmarks...${NC}"
echo ""

# Create benchmark results directory
BENCH_DIR="benchmark_results"
mkdir -p "$BENCH_DIR"
TIMESTAMP=$(date '+%Y%m%d_%H%M%S')
RESULT_FILE="$BENCH_DIR/benchmark_$TIMESTAMP.txt"

# Function to run benchmark and extract results
run_benchmark() {
    local test_name="$1"
    local executable="$2"
    
    echo -e "${BLUE}$test_name${NC}"
    
    cd build
    ./"$executable" > "../temp_benchmark.log" 2>&1
    
    # Extract performance metrics
    if grep -q "microseconds per message" "../temp_benchmark.log"; then
        local avg_time=$(grep "microseconds per message" "../temp_benchmark.log" | tail -1 | awk '{print $2}')
        echo "  Average time per message: ${avg_time} microseconds"
        echo "$test_name: ${avg_time} μs/message" >> "../$RESULT_FILE"
    fi
    
    if grep -q "messages/second" "../temp_benchmark.log"; then
        local throughput=$(grep "messages/second" "../temp_benchmark.log" | tail -1 | awk '{print $2}')
        echo "  Throughput: ${throughput} messages/second"
        echo "$test_name throughput: ${throughput} msg/s" >> "../$RESULT_FILE"
    fi
    
    if grep -q "messages in.*microseconds" "../temp_benchmark.log"; then
        local batch_info=$(grep "messages in.*microseconds" "../temp_benchmark.log" | tail -1)
        echo "  Batch performance: $batch_info"
        echo "$test_name batch: $batch_info" >> "../$RESULT_FILE"
    fi
    
    cd ..
    echo ""
}

# Initialize results file
cat > "$RESULT_FILE" << EOF
CLog Performance Benchmark Results
Date: $(date)
Platform: $(uname -s) $(uname -m)
Compiler: $(g++ --version | head -1 2>/dev/null || echo "Unknown")

Performance Metrics:
EOF

# Run benchmarks
echo "Starting benchmark suite..."
echo ""

run_benchmark "Embedded Simulation Performance" "tests/integration/test_embedded_simulation"
run_benchmark "Desktop Integration Performance" "tests/integration/test_desktop_integration"

# Additional quick benchmarks
echo -e "${BLUE}Quick Performance Test${NC}"

cd build

# Simple throughput test
cat > ../temp_perf_test.cpp << 'EOF'
#include <clog/log.hpp>
#include <chrono>
#include <iostream>

int main() {
    int count = 0;
    clog::Logger::setCallback([&count](clog::Level, const char*, const char*) {
        count++;
    });
    clog::Logger::setLevel(clog::Level::INFO);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; i++) {
        CLOG_INFO("Perf", "Message %d", i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "10000 messages in " << duration.count() << " microseconds" << std::endl;
    std::cout << "Average: " << (duration.count() / 10000.0) << " microseconds per message" << std::endl;
    std::cout << "Throughput: " << (10000.0 * 1000000.0 / duration.count()) << " messages/second" << std::endl;
    
    return 0;
}
EOF

g++ -std=c++17 -I../include -O2 ../temp_perf_test.cpp -o temp_perf_test
./temp_perf_test >> "../$RESULT_FILE"
./temp_perf_test

rm -f ../temp_perf_test.cpp temp_perf_test

cd ..

# Clean up
rm -f temp_benchmark.log

echo -e "${GREEN}Benchmark completed!${NC}"
echo "Results saved to: $RESULT_FILE"
echo ""

# Show summary
echo -e "${BLUE}Benchmark Summary:${NC}"
tail -10 "$RESULT_FILE"