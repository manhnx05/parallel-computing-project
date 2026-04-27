# QuickSort Parallel Benchmark Suite Makefile

# Compiler settings
CC = gcc
MPICC = mpicc
CFLAGS = -Wall -Wextra -O3 -std=c99

# Platform-specific OpenMP flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS - use libomp
    # Try to detect Homebrew prefix dynamically
    HOMEBREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/opt/homebrew")
    LIBOMP_PREFIX := $(HOMEBREW_PREFIX)/opt/libomp
    
    # Check if environment variables are set (from CI)
    ifdef LIBOMP_INCLUDE
        OMP_CFLAGS = -Xpreprocessor -fopenmp -I$(LIBOMP_INCLUDE)
        OMP_LDFLAGS = -lomp -L$(LIBOMP_LIB)
    else
        # Use detected or fallback paths
        OMP_CFLAGS = -Xpreprocessor -fopenmp -I$(LIBOMP_PREFIX)/include
        OMP_LDFLAGS = -lomp -L$(LIBOMP_PREFIX)/lib
    endif
    
    # Ensure we use gcc, not clang
    ifeq ($(CC),cc)
        CC = gcc-14
    endif
else
    # Linux - standard OpenMP
    OMP_CFLAGS = -fopenmp
    OMP_LDFLAGS = -fopenmp
endif

PTHREADFLAGS = -lpthread
MPIFLAGS = 

# Directories
SRCDIR = src
INCDIR = include
BINDIR = bin
OBJDIR = obj
TESTDIR = tests
SCRIPTDIR = scripts
RESULTSDIR = results

# Source files
COMMON_SRC = $(SRCDIR)/quicksort_common.c
SEQUENTIAL_SRC = $(SRCDIR)/sequential_quicksort.c
OPENMP_SRC = $(SRCDIR)/openmp_quicksort.c
PTHREADS_SRC = $(SRCDIR)/pthreads_quicksort.c
MPI_SRC = $(SRCDIR)/mpi_quicksort.c

# Object files
COMMON_OBJ = $(OBJDIR)/quicksort_common.o
SEQUENTIAL_OBJ = $(OBJDIR)/sequential_quicksort.o
OPENMP_OBJ = $(OBJDIR)/openmp_quicksort.o
PTHREADS_OBJ = $(OBJDIR)/pthreads_quicksort.o
MPI_OBJ = $(OBJDIR)/mpi_quicksort.o

# Executables
SEQUENTIAL_BIN = $(BINDIR)/sequential_quicksort
OPENMP_BIN = $(BINDIR)/openmp_quicksort
PTHREADS_BIN = $(BINDIR)/pthreads_quicksort
MPI_BIN = $(BINDIR)/mpi_quicksort

# Include path
INCLUDES = -I$(INCDIR)

# Default target
all: directories $(SEQUENTIAL_BIN) $(OPENMP_BIN) $(PTHREADS_BIN) $(MPI_BIN)

# Create necessary directories
directories:
	@mkdir -p $(BINDIR) $(OBJDIR) $(RESULTSDIR)

# Common object file
$(COMMON_OBJ): $(COMMON_SRC) $(INCDIR)/quicksort_common.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Sequential QuickSort
$(SEQUENTIAL_OBJ): $(SEQUENTIAL_SRC) $(INCDIR)/quicksort_common.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(SEQUENTIAL_BIN): $(SEQUENTIAL_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# OpenMP QuickSort
$(OPENMP_OBJ): $(OPENMP_SRC) $(INCDIR)/quicksort_common.h
	$(CC) $(CFLAGS) $(OMP_CFLAGS) $(INCLUDES) -c $< -o $@

$(OPENMP_BIN): $(OPENMP_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ $(OMP_LDFLAGS) -o $@

# Pthreads QuickSort
$(PTHREADS_OBJ): $(PTHREADS_SRC) $(INCDIR)/quicksort_common.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(PTHREADS_BIN): $(PTHREADS_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ $(PTHREADFLAGS) -o $@

# MPI QuickSort
$(MPI_OBJ): $(MPI_SRC) $(INCDIR)/quicksort_common.h
	$(MPICC) $(CFLAGS) $(MPIFLAGS) $(INCLUDES) -c $< -o $@

$(MPI_BIN): $(MPI_OBJ) $(COMMON_OBJ)
	$(MPICC) $(CFLAGS) $(MPIFLAGS) $^ -o $@

# Test targets
test: all
	@echo "Running basic functionality tests..."
	@$(SCRIPTDIR)/run_tests.sh

# Benchmark targets
benchmark: all
	@echo "Running comprehensive benchmark..."
	@$(SCRIPTDIR)/run_benchmark.sh

benchmark-quick: all
	@echo "Running quick benchmark..."
	@$(SCRIPTDIR)/run_benchmark.sh --quick

# Individual algorithm targets
sequential: directories $(SEQUENTIAL_BIN)
openmp: directories $(OPENMP_BIN)
pthreads: directories $(PTHREADS_BIN)
mpi: directories $(MPI_BIN)

# Clean targets
clean:
	rm -rf $(OBJDIR) $(BINDIR)

clean-results:
	rm -rf $(RESULTSDIR)/*

clean-all: clean clean-results

# Install target (optional)
install: all
	@echo "Installing binaries to /usr/local/bin (requires sudo)..."
	sudo cp $(BINDIR)/* /usr/local/bin/

# Help target
help:
	@echo "QuickSort Parallel Benchmark Suite"
	@echo "Available targets:"
	@echo "  all          - Build all executables"
	@echo "  sequential   - Build sequential version only"
	@echo "  openmp       - Build OpenMP version only"
	@echo "  pthreads     - Build Pthreads version only"
	@echo "  mpi          - Build MPI version only"
	@echo "  test         - Run functionality tests"
	@echo "  benchmark    - Run comprehensive benchmark"
	@echo "  benchmark-quick - Run quick benchmark"
	@echo "  clean        - Remove build files"
	@echo "  clean-results - Remove result files"
	@echo "  clean-all    - Remove all generated files"
	@echo "  install      - Install binaries system-wide"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all directories test benchmark benchmark-quick sequential openmp pthreads mpi clean clean-results clean-all install help