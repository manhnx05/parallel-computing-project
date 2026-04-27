# QuickSort Parallel Benchmark Suite

A comprehensive benchmark suite for comparing different parallel implementations of the QuickSort algorithm. This project provides implementations using Sequential, OpenMP, Pthreads, and MPI approaches with a unified benchmarking framework.

## 🚀 Features

- **Multiple Parallel Implementations**: Sequential, OpenMP, Pthreads, and MPI
- **Comprehensive Benchmarking**: Automated performance testing with configurable parameters
- **Professional Build System**: Makefile-based build with proper dependency management
- **Result Analysis**: CSV output and summary reports
- **Cross-Platform**: Works on Linux, macOS, and Windows (with appropriate toolchains)
- **Configurable Testing**: Flexible array sizes, thread counts, and test iterations

## 📁 Project Structure

```
├── src/                    # Source code files
│   ├── sequential_quicksort.c
│   ├── openmp_quicksort.c
│   ├── pthreads_quicksort.c
│   ├── mpi_quicksort.c
│   └── quicksort_common.c
├── include/                # Header files
│   └── quicksort_common.h
├── scripts/                # Benchmark and test scripts
│   ├── run_benchmark.sh
│   └── run_tests.sh
├── tests/                  # Test files (future expansion)
├── results/                # Benchmark results (generated)
├── docs/                   # Documentation
├── bin/                    # Compiled binaries (generated)
├── obj/                    # Object files (generated)
├── Makefile               # Build configuration
└── README.md              # This file
```

## 🛠️ Prerequisites

### Required
- **GCC**: C compiler with C99 support
- **Make**: Build system

### Optional (for specific implementations)
- **OpenMP**: Usually included with GCC
- **Pthreads**: POSIX threads (standard on Unix-like systems)
- **MPI**: Message Passing Interface (OpenMPI, MPICH, or MS-MPI)

### Installation Examples

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential libopenmpi-dev
```

**CentOS/RHEL:**
```bash
sudo yum groupinstall "Development Tools"
sudo yum install openmpi-devel
```

**macOS:**
```bash
brew install gcc open-mpi
```

**Windows:**
- Install MinGW-w64 or Visual Studio
- Install Microsoft MPI for MPI support

## 🔧 Building

### Build All Implementations
```bash
make all
```

### Build Individual Implementations
```bash
make sequential    # Sequential version only
make openmp       # OpenMP version only
make pthreads     # Pthreads version only
make mpi          # MPI version only
```

### Clean Build Files
```bash
make clean        # Remove build files
make clean-all    # Remove build files and results
```

## 🏃‍♂️ Running

### Individual Algorithms

**Sequential QuickSort:**
```bash
./bin/sequential_quicksort --size 1000000 --runs 3
```

**OpenMP QuickSort:**
```bash
./bin/openmp_quicksort --size 1000000 --threads 4 --runs 3
```

**Pthreads QuickSort:**
```bash
./bin/pthreads_quicksort --size 1000000 --threads 4 --runs 3
```

**MPI QuickSort:**
```bash
mpiexec -n 4 ./bin/mpi_quicksort --size 1000000 --runs 3
```

### Command Line Options

All implementations support these common options:

- `-s, --size SIZE`: Array size (default: 10,000,000)
- `-r, --runs NUM`: Number of benchmark runs (default: 1)
- `--no-verify`: Skip result verification
- `--csv [FILE]`: Output results to CSV file
- `-h, --help`: Show help message

**Thread-based implementations also support:**
- `-t, --threads NUM`: Number of threads (default: 4)

## 📊 Benchmarking

### Run Complete Benchmark Suite
```bash
make benchmark
```

### Run Quick Benchmark
```bash
make benchmark-quick
```

### Custom Benchmark
```bash
./scripts/run_benchmark.sh --sizes 1000000,5000000 --threads 1,2,4,8 --runs 5
```

### Benchmark Options
- `--quick`: Run with smaller parameters for faster testing
- `--sizes SIZE1,SIZE2`: Comma-separated array sizes
- `--threads T1,T2`: Comma-separated thread counts
- `--runs NUM`: Number of runs per test
- `--no-csv`: Don't output CSV results

## 🧪 Testing

### Run Functionality Tests
```bash
make test
```

This runs basic functionality tests to ensure all implementations work correctly.

## 📈 Results Analysis

Benchmark results are saved in the `results/` directory with timestamps. Each CSV file contains:

- **Algorithm**: Implementation type (Sequential, OpenMP, Pthreads, MPI)
- **ArraySize**: Number of elements sorted
- **Threads**: Number of threads/processes used
- **Time**: Execution time in seconds
- **Speedup**: Speedup compared to sequential implementation

### Example Results Analysis

```bash
# View latest results
ls -la results/

# Analyze specific result file
cat results/benchmark_results_20240427_123456.csv
```

## 🔍 Algorithm Details

### Sequential QuickSort
- Classic divide-and-conquer implementation
- Serves as baseline for performance comparison
- Supports both ascending and descending sort

### OpenMP QuickSort
- Uses OpenMP tasks for parallel recursion
- Automatic load balancing
- Depth-limited parallelization to avoid overhead

### Pthreads QuickSort
- Manual thread management with POSIX threads
- Array partitioning approach
- Merge-based result combination

### MPI QuickSort
- Distributed memory parallelization
- Process-based data distribution
- Suitable for cluster computing

## 🎯 Performance Tips

1. **Array Size**: Larger arrays show better parallel efficiency
2. **Thread Count**: Optimal thread count usually matches CPU cores
3. **System Load**: Run benchmarks on idle systems for consistent results
4. **Memory**: Ensure sufficient RAM for large arrays
5. **NUMA**: Consider NUMA topology for multi-socket systems

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📚 References

- Hoare, C.A.R. (1962). "Quicksort". The Computer Journal. 5 (1): 10–16.
- OpenMP Architecture Review Board. OpenMP Application Programming Interface.
- IEEE Std 1003.1-2017 (POSIX.1-2017). IEEE Standard for Information Technology.
- MPI Forum. MPI: A Message-Passing Interface Standard.

## 🙋‍♂️ Support

If you encounter any issues or have questions:

1. Check the [Issues](../../issues) page
2. Create a new issue with detailed information
3. Include system information and error messages

## 📊 Benchmark Results Example

| Algorithm  | Array Size | Threads | Time (s) | Speedup |
|------------|------------|---------|----------|---------|
| Sequential | 10M        | 1       | 2.456    | 1.00x   |
| OpenMP     | 10M        | 4       | 0.687    | 3.57x   |
| Pthreads   | 10M        | 4       | 0.723    | 3.40x   |
| MPI        | 10M        | 4       | 0.891    | 2.76x   |