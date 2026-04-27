#ifndef QUICKSORT_COMMON_H
#define QUICKSORT_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Configuration constants
#define DEFAULT_ARRAY_SIZE 10000000
#define MAX_ARRAY_SIZE 100000000
#define MIN_ARRAY_SIZE 1000
#define MAX_THREADS 64
#define MIN_THREADS 1

// Benchmark configuration
typedef struct {
    int array_size;
    int num_threads;
    int num_runs;
    int verify_result;
    int output_csv;
    char* output_file;
} BenchmarkConfig;

// Timing utilities
typedef struct {
    double start_time;
    double end_time;
    double elapsed_time;
} Timer;

// Common function prototypes
extern void swap(int* a, int* b);
extern int partition(int arr[], int left, int right);
extern void generate_random_array(int arr[], int n);
extern void copy_array(const int src[], int dest[], int n);
extern int verify_sorted(const int arr[], int n, int ascending);

// Timing functions
extern void timer_start(Timer* timer);
extern void timer_end(Timer* timer);
extern double get_elapsed_time(const Timer* timer);

// Benchmark utilities

extern void save_benchmark_csv(const char* filename, const char* algorithm, int array_size, int threads, double time, double speedup);

// Memory utilities
extern int* allocate_array(int size);
extern void free_array(int* arr);

// Configuration parsing
extern BenchmarkConfig parse_arguments(int argc, char* argv[]);
extern void print_usage(const char* program_name);

#endif // QUICKSORT_COMMON_H