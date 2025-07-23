#!/bin/bash

# Quick Test Script for CLog
# Fast smoke test for development workflow

set -e

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}=== CLog Quick Test ===${NC}"

# Build if needed
if [ ! -d "build" ] || [ ! -f "build/Makefile" ]; then
    echo "Building..."
    mkdir -p build
    cd build
    cmake -DCLOG_BUILD_TESTS=ON -DCLOG_BUILD_EXAMPLES=ON .. > /dev/null
    make > /dev/null
    cd ..
fi

# Run a subset of critical tests
echo "Running critical tests..."

cd build

# Test logger core functionality
echo -n "Logger core: "
if ./tests/unit/test_logger > /dev/null 2>&1; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    exit 1
fi

# Test platform detection
echo -n "Platform detection: "
if ./tests/unit/test_platform > /dev/null 2>&1; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    exit 1
fi

# Test desktop example runs
echo -n "Desktop example: "
if ./examples/desktop/desktop_example > /dev/null 2>&1; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗${NC}"
    exit 1
fi

cd ..

echo -e "${GREEN}🚀 Quick test passed!${NC}"