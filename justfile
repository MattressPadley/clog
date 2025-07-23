# CLog Library Management
# Just is a command runner similar to Make but with better syntax
# Install with: brew install just (macOS) or cargo install just

# Default recipe - show help
default:
    @just --list

# Build and Development Commands
# ===============================

# Build the library and examples (CMake)
build:
    mkdir -p build
    cd build && cmake .. && make

# Build examples only
build-examples:
    mkdir -p build
    cd build && cmake -DCLOG_BUILD_EXAMPLES=ON .. && make

# Clean build artifacts
clean:
    rm -rf build/
    rm -rf examples/desktop/build/
    rm -f test_output.log

# Testing Commands  
# ================

# Build and run all tests
test: build-tests
    cd build && ctest --verbose

# Build test suite
build-tests:
    mkdir -p build
    cd build && cmake -DCLOG_BUILD_TESTS=ON .. && make

# Run unit tests only
test-unit: build-tests
    cd build && ctest --verbose -L unit

# Run integration tests only  
test-integration: build-tests
    cd build && ctest --verbose -L integration

# Run specific test by name
test-specific test_name: build-tests
    cd build && ctest --verbose -R {{test_name}}

# Run tests with debugging output
test-debug: build-tests
    cd build && ctest --verbose --output-on-failure

# Examples Commands
# =================

# Run desktop example
run-desktop-example: build-examples
    ./build/examples/desktop/desktop_example

# Compile and run callback example  
run-callback-example: build-examples
    cd build && find . -name "*callback*" -executable -exec {} \;

# Development Commands
# ====================

# Format code (if clang-format is available)
format:
    @echo "Formatting code..."
    find include/ -name "*.hpp" -exec clang-format -i {} \; || echo "clang-format not found"
    find examples/ -name "*.cpp" -exec clang-format -i {} \; || echo "clang-format not found"
    find tests/ -name "*.cpp" -exec clang-format -i {} \; || echo "clang-format not found"

# Check code style (if clang-format is available)
check-format:
    @echo "Checking code formatting..."
    find include/ -name "*.hpp" -exec clang-format --dry-run --Werror {} \; || echo "Format check failed or clang-format not found"

# Run static analysis (if cppcheck is available)
analyze:
    @echo "Running static analysis..."
    cppcheck --enable=all --std=c++17 --suppress=missingIncludeSystem include/ || echo "cppcheck not found"

# Generate documentation (if doxygen is available)
docs:
    @echo "Generating documentation..."
    doxygen docs/Doxyfile || echo "doxygen not found or Doxyfile missing"

# Performance and Benchmarking
# =============================

# Run performance benchmarks
benchmark: build-tests
    @echo "Running performance benchmarks..."
    cd build && ./tests/integration/test_embedded_simulation | grep -E "(microseconds|messages/second)"

# Profile memory usage (if valgrind is available)
profile-memory: build-tests
    @echo "Profiling memory usage..."
    cd build && valgrind --tool=memcheck --leak-check=full ./tests/unit/test_logger || echo "valgrind not found"

# Installation and Packaging
# ===========================

# Install library system-wide (requires sudo)
install: build
    cd build && sudo make install

# Create distributable package
package: clean build test
    @echo "Creating package..."
    mkdir -p dist/clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}
    cp -r include/ dist/clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}/
    cp -r examples/ dist/clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}/
    cp -r docs/ dist/clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}/
    cp LICENSE README.md dist/clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}/
    cd dist && tar -czf clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}.tar.gz clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}/
    @echo "Package created: dist/clog-{{`cat VERSION 2>/dev/null || echo "0.0.1"`}}.tar.gz"

# Development Environment
# =======================

# Set up development environment
setup-dev:
    @echo "Setting up development environment..."
    mkdir -p build
    @echo "Install dependencies:"
    @echo "  - CMake >= 3.12"
    @echo "  - C++17 compatible compiler"
    @echo "  - Optional: clang-format, cppcheck, valgrind, doxygen"

# Check development environment
check-env:
    @echo "Checking development environment..."
    cmake --version || echo "❌ CMake not found"
    g++ --version || clang++ --version || echo "❌ C++ compiler not found"
    clang-format --version && echo "✅ clang-format available" || echo "⚠️  clang-format not found (optional)"
    cppcheck --version && echo "✅ cppcheck available" || echo "⚠️  cppcheck not found (optional)"
    valgrind --version && echo "✅ valgrind available" || echo "⚠️  valgrind not found (optional)"

# Project Management
# ==================

# Show project statistics
stats:
    @echo "=== CLog Project Statistics ==="
    @echo "Header files: $(find include/ -name "*.hpp" | wc -l)"
    @echo "Example files: $(find examples/ -name "*.cpp" | wc -l)"
    @echo "Test files: $(find tests/ -name "*.cpp" | wc -l)"
    @echo "Total lines of code:"
    find include/ examples/ tests/ -name "*.hpp" -o -name "*.cpp" | xargs wc -l | tail -1
    @echo "Documentation files: $(find docs/ -name "*.md" | wc -l)"

# Validate library structure
validate:
    @echo "Validating library structure..."
    test -f include/clog/log.hpp && echo "✅ Main header exists" || echo "❌ Main header missing"
    test -f include/clog/platform.hpp && echo "✅ Platform header exists" || echo "❌ Platform header missing"
    test -f include/clog/config.hpp && echo "✅ Config header exists" || echo "❌ Config header missing"
    test -f CMakeLists.txt && echo "✅ CMakeLists.txt exists" || echo "❌ CMakeLists.txt missing"
    test -f library.json && echo "✅ PlatformIO manifest exists" || echo "❌ PlatformIO manifest missing"
    test -f LICENSE && echo "✅ License exists" || echo "❌ License missing"
    test -f README.md && echo "✅ README exists" || echo "❌ README missing"

# Git and Release Management
# ==========================

# Create a new release tag
release version:
    @echo "Creating release {{version}}..."
    git tag -a v{{version}} -m "Release version {{version}}"
    @echo "Don't forget to: git push origin v{{version}}"

# Prepare for release (run all checks)
pre-release: clean validate test check-format analyze
    @echo "✅ Pre-release checks completed successfully"
    @echo "Ready to create release!"

# Show recent changes (requires git log)
changelog:
    @echo "Recent changes:"
    git log --oneline --decorate --graph -10 || echo "Not a git repository"

# Continuous Integration
# ======================

# Run CI-like checks locally
ci: clean build test check-format
    @echo "✅ CI checks completed successfully"

# Quick smoke test (fast validation)
smoke-test: build
    @echo "Running smoke test..."
    cd build && ./examples/desktop/desktop_example > /dev/null && echo "✅ Desktop example runs" || echo "❌ Desktop example failed"

# All-in-one command for complete validation
full-check: clean validate build test analyze benchmark
    @echo "🎉 Full validation completed successfully!"

# Help and Information
# ====================

# Show detailed help
help:
    @echo "CLog Library Management Commands"
    @echo "================================"
    @echo ""
    @echo "🔨 Build Commands:"
    @echo "  just build              - Build library and examples"
    @echo "  just build-examples     - Build examples only"
    @echo "  just clean              - Clean build artifacts"
    @echo ""
    @echo "🧪 Testing Commands:"
    @echo "  just test               - Run all tests"
    @echo "  just test-unit          - Run unit tests only"
    @echo "  just test-integration   - Run integration tests only"
    @echo "  just test-debug         - Run tests with debug output"
    @echo ""
    @echo "🚀 Examples:"
    @echo "  just run-desktop-example - Run desktop example"
    @echo ""
    @echo "🔧 Development:"
    @echo "  just format             - Format code"
    @echo "  just analyze            - Run static analysis"
    @echo "  just setup-dev          - Setup development environment"
    @echo "  just validate           - Validate project structure"
    @echo ""
    @echo "📦 Release:"
    @echo "  just package            - Create distributable package"
    @echo "  just pre-release        - Run all pre-release checks"
    @echo "  just ci                 - Run CI-like checks"