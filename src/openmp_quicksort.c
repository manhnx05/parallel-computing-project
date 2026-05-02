#include "../include/quicksort_common.h"
#ifdef _OPENMP
#include <omp.h>
#endif

#define N DEFAULT_ARRAY_SIZE 



void quicksort_parallel(int arr[], int left, int right, int depth) {
    if (left < right) {
        int pi = partition(arr, left, right);

#ifdef _OPENMP
        if (depth < 4) {
            #pragma omp task shared(arr)
            quicksort_parallel(arr, left, pi - 1, depth + 1);

            #pragma omp task shared(arr)
            quicksort_parallel(arr, pi + 1, right, depth + 1);

            #pragma omp taskwait
        } else {
            quicksort_parallel(arr, left, pi - 1, depth + 1);
            quicksort_parallel(arr, pi + 1, right, depth + 1);
        }
#else
        // Sequential fallback
        quicksort_parallel(arr, left, pi - 1, depth + 1);
        quicksort_parallel(arr, pi + 1, right, depth + 1);
#endif
    }
}

int main(int argc, char* argv[]) {
    BenchmarkConfig config = parse_arguments(argc, argv);
    
    int* arr = allocate_array(config.array_size);
    
    printf("OpenMP QuickSort Benchmark\n");
    printf("Array size: %d elements\n", config.array_size);
    printf("Number of threads: %d\n", config.num_threads);
    printf("Number of runs: %d\n\n", config.num_runs);

    generate_random_array(arr, config.array_size);
    
#ifdef _OPENMP
    omp_set_num_threads(config.num_threads);
#else
    printf("Warning: OpenMP not available, running sequentially\n");
#endif

    double total_time = 0.0;
    Timer timer;

    for (int run = 0; run < config.num_runs; run++) {
        // Regenerate array for each run to ensure consistency
        if (run > 0) {
            generate_random_array(arr, config.array_size);
        }

        timer_start(&timer);

#ifdef _OPENMP
        #pragma omp parallel
        {
            #pragma omp single
            quicksort_parallel(arr, 0, config.array_size - 1, 0);
        }
#else
        // Fallback to sequential quicksort when OpenMP not available
        quicksort_parallel(arr, 0, config.array_size - 1, 0);
#endif

        timer_end(&timer);
        double elapsed = get_elapsed_time(&timer);
        total_time += elapsed;

        if (config.verify_result && !verify_sorted(arr, config.array_size, 1)) {
            printf("Error: Sort failed verification on run %d!\n", run + 1);
            return 1;
        }

        if (config.num_runs > 1) {
            printf("Run %d: %.6f seconds\n", run + 1, elapsed);
        }
    }

    double avg_time = total_time / config.num_runs;
    printf("\nResults:\n");
    printf("Average time: %.6f seconds\n", avg_time);
    printf("Throughput: %.2f M elements/second\n", config.array_size / (avg_time * 1000000));

    if (config.output_csv) {
        save_benchmark_csv(config.output_file, "OpenMP", config.array_size, config.num_threads, avg_time, 0.0);
    }

    free_array(arr);
    return 0;
}
