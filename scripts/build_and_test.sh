#!/bin/bash

# Build and Test Script for QuickSort Benchmark Suite

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "QuickSort Benchmark Suite - Build & Test"
echo "=========================================="

cd "$PROJECT_DIR"

# Clean previous builds
echo "Cleaning previous builds..."
make clean-all

# Build all targets
echo "Building all targets..."
make all

# Run tests
echo "Running functionality tests..."
make test

# Run quick benchmark
echo "Running quick benchmark..."
make benchmark-quick

echo "=========================================="
echo "Build and test completed successfully!"
echo "=========================================="