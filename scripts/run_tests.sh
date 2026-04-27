#!/bin/bash

# QuickSort Test Runner
# This script runs basic functionality tests for all implementations

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BIN_DIR="$PROJECT_DIR/bin"

echo "=========================================="
echo "QuickSort Functionality Tests"
echo "=========================================="

# Test parameters
TEST_SIZE=10000
TEST_THREADS=2

# Function to test an algorithm
test_algorithm() {
    local name=$1
    local binary=$2
    local use_mpi=$3
    
    echo "Testing $name..."
    
    if [ ! -f "$binary" ]; then
        echo "  SKIP: Binary not found: $binary"
        return
    fi
    
    if [ "$use_mpi" = true ]; then
        if ! command -v mpiexec >/dev/null 2>&1; then
            echo "  SKIP: MPI not available"
            return
        fi
        cmd="mpiexec -n $TEST_THREADS $binary -s $TEST_SIZE -r 1"
    else
        cmd="$binary -s $TEST_SIZE -t $TEST_THREADS -r 1"
    fi
    
    echo "  Running: $cmd"
    if eval "$cmd" >/dev/null 2>&1; then
        echo "  PASS: $name test completed successfully"
    else
        echo "  FAIL: $name test failed"
        return 1
    fi
}

# Run tests
test_algorithm "Sequential" "$BIN_DIR/sequential_quicksort" false
test_algorithm "OpenMP" "$BIN_DIR/openmp_quicksort" false  
test_algorithm "Pthreads" "$BIN_DIR/pthreads_quicksort" false
test_algorithm "MPI" "$BIN_DIR/mpi_quicksort" true

echo "=========================================="
echo "All tests completed!"
echo "=========================================="