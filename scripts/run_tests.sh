#!/bin/bash

# CLog Test Runner Script
# Comprehensive test execution with reporting

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
TEST_RESULTS_DIR="test_results"
TIMESTAMP=$(date '+%Y%m%d_%H%M%S')

echo -e "${BLUE}=== CLog Test Runner ===${NC}"
echo "Starting test execution at $(date)"

# Create directories
mkdir -p "$BUILD_DIR"
mkdir -p "$TEST_RESULTS_DIR"

# Function to print section headers
print_section() {
    echo ""
    echo -e "${BLUE}=== $1 ===${NC}"
}

# Function to check if command succeeded
check_result() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ $1 passed${NC}"
        return 0
    else
        echo -e "${RED}✗ $1 failed${NC}"
        return 1
    fi
}

# Build the tests
print_section "Building Tests"
cd "$BUILD_DIR"
cmake -DCLOG_BUILD_TESTS=ON .. > "../$TEST_RESULTS_DIR/cmake_output_$TIMESTAMP.log" 2>&1
check_result "CMake configuration"

make > "../$TEST_RESULTS_DIR/build_output_$TIMESTAMP.log" 2>&1
check_result "Build"

cd ..

# Run unit tests
print_section "Running Unit Tests"
cd "$BUILD_DIR"
echo "Running test_logger..."
./tests/unit/test_logger > "../$TEST_RESULTS_DIR/test_logger_$TIMESTAMP.log" 2>&1
check_result "test_logger"

echo "Running test_platform..."
./tests/unit/test_platform > "../$TEST_RESULTS_DIR/test_platform_$TIMESTAMP.log" 2>&1
check_result "test_platform"

cd ..

# Run integration tests  
print_section "Running Integration Tests"
cd "$BUILD_DIR"
echo "Running test_desktop_integration..."
./tests/integration/test_desktop_integration > "../$TEST_RESULTS_DIR/test_desktop_integration_$TIMESTAMP.log" 2>&1
check_result "test_desktop_integration"

echo "Running test_embedded_simulation..."
./tests/integration/test_embedded_simulation > "../$TEST_RESULTS_DIR/test_embedded_simulation_$TIMESTAMP.log" 2>&1
check_result "test_embedded_simulation"

cd ..

# Run CTest for comprehensive results
print_section "Running CTest"
cd "$BUILD_DIR"
ctest --verbose --output-on-failure > "../$TEST_RESULTS_DIR/ctest_output_$TIMESTAMP.log" 2>&1
if check_result "CTest execution"; then
    CTEST_RESULT="PASSED"
else
    CTEST_RESULT="FAILED"
fi
cd ..

# Generate summary report
print_section "Test Summary"
SUMMARY_FILE="$TEST_RESULTS_DIR/test_summary_$TIMESTAMP.txt"

cat > "$SUMMARY_FILE" << EOF
CLog Library Test Summary
========================
Date: $(date)
Build Directory: $BUILD_DIR
Results Directory: $TEST_RESULTS_DIR

Test Results:
EOF

# Count test results from log files
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

for log_file in "$TEST_RESULTS_DIR"/test_*_$TIMESTAMP.log; do
    if [ -f "$log_file" ]; then
        test_name=$(basename "$log_file" .log | sed "s/_$TIMESTAMP//")
        
        # Count passed/failed tests in the log
        passed=$(grep -c "✓" "$log_file" 2>/dev/null || echo "0")
        failed=$(grep -c "✗" "$log_file" 2>/dev/null || echo "0")
        
        echo "  $test_name: $passed passed, $failed failed" >> "$SUMMARY_FILE"
        
        TOTAL_TESTS=$((TOTAL_TESTS + passed + failed))
        PASSED_TESTS=$((PASSED_TESTS + passed))
        FAILED_TESTS=$((FAILED_TESTS + failed))
    fi
done

cat >> "$SUMMARY_FILE" << EOF

Overall Statistics:
  Total Tests: $TOTAL_TESTS
  Passed: $PASSED_TESTS
  Failed: $FAILED_TESTS
  Success Rate: $(echo "scale=1; $PASSED_TESTS * 100 / $TOTAL_TESTS" | bc -l 2>/dev/null || echo "N/A")%

CTest Result: $CTEST_RESULT

Log Files Generated:
EOF

ls -la "$TEST_RESULTS_DIR"/*_$TIMESTAMP.* >> "$SUMMARY_FILE"

echo ""
echo -e "${BLUE}=== Final Results ===${NC}"
cat "$SUMMARY_FILE"

# Clean up temporary files (optional)
if [ "$1" = "--clean" ]; then
    print_section "Cleaning Up"
    rm -f test_output.log  # Remove any test artifacts
    echo "Temporary files cleaned"
fi

# Exit with appropriate code
if [ "$FAILED_TESTS" -eq 0 ] && [ "$CTEST_RESULT" = "PASSED" ]; then
    echo -e "${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}❌ Some tests failed. Check log files for details.${NC}"
    exit 1
fi