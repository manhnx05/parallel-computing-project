#include "../include/quicksort_common.h"
#include <sys/time.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int left, int right) {
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[right]);
    return i + 1;
}

void generate_random_array(int arr[], int n) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand();
    }
}

void copy_array(int src[], int dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int verify_sorted(int arr[], int n, int ascending) {
    for (int i = 1; i < n; i++) {
        if (ascending) {
            if (arr[i - 1] > arr[i]) {
                printf("Error: Array not sorted correctly at position %d\n", i - 1);
                return 0;
            }
        } else {
            if (arr[i - 1] < arr[i]) {
                printf("Error: Array not sorted correctly at position %d\n", i - 1);
                return 0;
            }
        }
    }
    return 1;
}

void print_array(int arr[], int n, int max_print) {
    int limit = (n < max_print) ? n : max_print;
    printf("Array: ");
    for (int i = 0; i < limit; i++) {
        printf("%d ", arr[i]);
    }
    if (n > max_print) {
        printf("... (%d more elements)", n - max_print);
    }
    printf("\n");
}

void timer_start(Timer* timer) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    timer->start_time = tv.tv_sec + tv.tv_usec / 1000000.0;
}

void timer_end(Timer* timer) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    timer->end_time = tv.tv_sec + tv.tv_usec / 1000000.0;
    timer->elapsed_time = timer->end_time - timer->start_time;
}

double get_elapsed_time(Timer* timer) {
    return timer->elapsed_time;
}

void print_benchmark_header(void) {
    printf("=================================================================\n");
    printf("                QuickSort Parallel Benchmark Suite\n");
    printf("=================================================================\n");
    printf("%-15s %-12s %-8s %-12s %-10s\n", "Algorithm", "Array Size", "Threads", "Time (s)", "Speedup");
    printf("-----------------------------------------------------------------\n");
}

void print_benchmark_result(const char* algorithm, int array_size, int threads, double time, double speedup) {
    printf("%-15s %-12d %-8d %-12.6f %-10.2fx\n", algorithm, array_size, threads, time, speedup);
}

void save_benchmark_csv(const char* filename, const char* algorithm, int array_size, int threads, double time, double speedup) {
    FILE* file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%s,%d,%d,%.6f,%.2f\n", algorithm, array_size, threads, time, speedup);
        fclose(file);
    }
}

int* allocate_array(int size) {
    int* arr = (int*)malloc(size * sizeof(int));
    if (!arr) {
        printf("Error: Cannot allocate memory for array of size %d\n", size);
        exit(1);
    }
    return arr;
}

void free_array(int* arr) {
    if (arr) {
        free(arr);
    }
}

BenchmarkConfig parse_arguments(int argc, char* argv[]) {
    BenchmarkConfig config = {
        .array_size = DEFAULT_ARRAY_SIZE,
        .num_threads = 4,
        .num_runs = 1,
        .verify_result = 1,
        .output_csv = 0,
        .output_file = NULL
    };

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) {
            if (i + 1 < argc) {
                config.array_size = atoi(argv[++i]);
                if (config.array_size < MIN_ARRAY_SIZE || config.array_size > MAX_ARRAY_SIZE) {
                    printf("Warning: Array size should be between %d and %d\n", MIN_ARRAY_SIZE, MAX_ARRAY_SIZE);
                }
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
            if (i + 1 < argc) {
                config.num_threads = atoi(argv[++i]);
                if (config.num_threads < MIN_THREADS || config.num_threads > MAX_THREADS) {
                    printf("Warning: Number of threads should be between %d and %d\n", MIN_THREADS, MAX_THREADS);
                }
            }
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--runs") == 0) {
            if (i + 1 < argc) {
                config.num_runs = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "--no-verify") == 0) {
            config.verify_result = 0;
        } else if (strcmp(argv[i], "--csv") == 0) {
            config.output_csv = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                config.output_file = argv[++i];
            } else {
                config.output_file = "benchmark_results.csv";
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        }
    }

    return config;
}

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -s, --size SIZE      Array size (default: %d)\n", DEFAULT_ARRAY_SIZE);
    printf("  -t, --threads NUM    Number of threads (default: 4)\n");
    printf("  -r, --runs NUM       Number of benchmark runs (default: 1)\n");
    printf("  --no-verify          Skip result verification\n");
    printf("  --csv [FILE]         Output results to CSV file (default: benchmark_results.csv)\n");
    printf("  -h, --help           Show this help message\n");
}