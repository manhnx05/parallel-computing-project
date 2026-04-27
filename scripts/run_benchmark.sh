#!/bin/bash

# QuickSort Parallel Benchmark Runner
# This script runs comprehensive benchmarks for all QuickSort implementations

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BIN_DIR="$PROJECT_DIR/bin"
RESULTS_DIR="$PROJECT_DIR/results"

# Default parameters
ARRAY_SIZES=(1000000 5000000 10000000)
THREAD_COUNTS=(1 2 4 8)
NUM_RUNS=3
QUICK_MODE=false
OUTPUT_CSV=true

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --quick)
            QUICK_MODE=true
            ARRAY_SIZES=(1000000)
            THREAD_COUNTS=(1 2 4)
            NUM_RUNS=1
            shift
            ;;
        --sizes)
            IFS=',' read -ra ARRAY_SIZES <<< "$2"
            shift 2
            ;;
        --threads)
            IFS=',' read -ra THREAD_COUNTS <<< "$2"
            shift 2
            ;;
        --runs)
            NUM_RUNS="$2"
            shift 2
            ;;
        --no-csv)
            OUTPUT_CSV=false
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --quick           Run quick benchmark with smaller parameters"
            echo "  --sizes SIZE1,SIZE2  Comma-separated array sizes (default: 1M,5M,10M)"
            echo "  --threads T1,T2   Comma-separated thread counts (default: 1,2,4,8)"
            echo "  --runs NUM        Number of runs per test (default: 3)"
            echo "  --no-csv          Don't output CSV results"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create results directory
mkdir -p "$RESULTS_DIR"

# Generate timestamp for this benchmark run
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULT_FILE="$RESULTS_DIR/benchmark_results_$TIMESTAMP.csv"

echo "=========================================="
echo "QuickSort Parallel Benchmark Suite"
echo "=========================================="
echo "Timestamp: $(date)"
echo "Array sizes: ${ARRAY_SIZES[*]}"
echo "Thread counts: ${THREAD_COUNTS[*]}"
echo "Runs per test: $NUM_RUNS"
echo "Results file: $RESULT_FILE"
echo "=========================================="

# Initialize CSV file
if [ "$OUTPUT_CSV" = true ]; then
    echo "Algorithm,ArraySize,Threads,Time,Speedup" > "$RESULT_FILE"
fi

# Function to run benchmark for a specific algorithm
run_algorithm_benchmark() {
    local algorithm=$1
    local binary=$2
    local use_mpi=$3
    
    echo ""
    echo "Benchmarking $algorithm..."
    echo "----------------------------------------"
    
    for size in "${ARRAY_SIZES[@]}"; do
        echo "Array size: $size"
        
        # Get sequential baseline for speedup calculation
        sequential_time=""
        if [ "$algorithm" != "Sequential" ]; then
            if [ -f "$BIN_DIR/sequential_quicksort" ]; then
                echo "  Getting sequential baseline..."
                sequential_time=$("$BIN_DIR/sequential_quicksort" -s "$size" -r 1 --no-verify | grep "Overall average time" | awk '{print $4}')
            fi
        fi
        
        for threads in "${THREAD_COUNTS[@]}"; do
            # Skip multi-threading for sequential algorithm
            if [ "$algorithm" = "Sequential" ] && [ "$threads" -gt 1 ]; then
                continue
            fi
            
            echo "  Threads: $threads"
            
            # Prepare command
            if [ "$use_mpi" = true ]; then
                cmd="mpiexec -n $threads $binary -s $size -r $NUM_RUNS --no-verify"
            else
                cmd="$binary -s $size -t $threads -r $NUM_RUNS --no-verify"
            fi
            
            # Add CSV output if enabled
            if [ "$OUTPUT_CSV" = true ]; then
                cmd="$cmd --csv $RESULT_FILE"
            fi
            
            # Run the benchmark
            echo "    Running: $cmd"
            if ! eval "$cmd"; then
                echo "    ERROR: Failed to run $algorithm benchmark"
                continue
            fi
            
            # Calculate and display speedup
            if [ -n "$sequential_time" ] && [ "$algorithm" != "Sequential" ]; then
                current_time=$(tail -1 "$RESULT_FILE" | cut -d',' -f4)
                if [ -n "$current_time" ] && [ "$current_time" != "0" ]; then
                    speedup=$(echo "scale=2; $sequential_time / $current_time" | bc -l 2>/dev/null || echo "N/A")
                    echo "    Speedup vs Sequential: ${speedup}x"
                fi
            fi
        done
    done
}

# Check if binaries exist
missing_binaries=()
for binary in sequential_quicksort openmp_quicksort pthreads_quicksort mpi_quicksort; do
    if [ ! -f "$BIN_DIR/$binary" ]; then
        missing_binaries+=("$binary")
    fi
done

if [ ${#missing_binaries[@]} -gt 0 ]; then
    echo "ERROR: Missing binaries: ${missing_binaries[*]}"
    echo "Please run 'make all' first to build all executables."
    exit 1
fi

# Run benchmarks for each algorithm
run_algorithm_benchmark "Sequential" "$BIN_DIR/sequential_quicksort" false
run_algorithm_benchmark "OpenMP" "$BIN_DIR/openmp_quicksort" false
run_algorithm_benchmark "Pthreads" "$BIN_DIR/pthreads_quicksort" false

# Check if MPI is available
if command -v mpiexec >/dev/null 2>&1; then
    run_algorithm_benchmark "MPI" "$BIN_DIR/mpi_quicksort" true
else
    echo ""
    echo "WARNING: MPI not found, skipping MPI benchmark"
fi

echo ""
echo "=========================================="
echo "Benchmark completed!"
echo "Results saved to: $RESULT_FILE"
echo "=========================================="

# Generate summary report
if [ "$OUTPUT_CSV" = true ] && [ -f "$RESULT_FILE" ]; then
    echo ""
    echo "Summary Report:"
    echo "---------------"
    
    # Best performance for each array size
    for size in "${ARRAY_SIZES[@]}"; do
        echo "Array size $size:"
        grep ",$size," "$RESULT_FILE" | sort -t',' -k4 -n | head -1 | \
        awk -F',' '{printf "  Best: %s with %d threads - %.4f seconds\n", $1, $3, $4}'
    done
fi