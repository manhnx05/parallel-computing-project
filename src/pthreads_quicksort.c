#include "../include/quicksort_common.h"
#include <pthread.h>

#define N DEFAULT_ARRAY_SIZE

typedef struct {
    int *arr;
    int left;
    int right;
} ThreadData;



void quicksort(int arr[], int left, int right) {
    if (left < right) {
        int pi = partition(arr, left, right);
        quicksort(arr, left, pi - 1);
        quicksort(arr, pi + 1, right);
    }
}

void *threaded_quicksort(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    quicksort(data->arr, data->left, data->right);
    pthread_exit(NULL);
}

void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int i = 0; i < n2; i++) {
        R[i] = arr[mid + 1 + i];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

int main(int argc, char* argv[]) {
    BenchmarkConfig config = parse_arguments(argc, argv);
    
    if (config.num_threads < 1 || config.num_threads > MAX_THREADS) {
        printf("Error: Number of threads should be between %d and %d\n", MIN_THREADS, MAX_THREADS);
        return 1;
    }

    int* arr = allocate_array(config.array_size);
    
    printf("Pthreads QuickSort Benchmark\n");
    printf("Array size: %d elements\n", config.array_size);
    printf("Number of threads: %d\n", config.num_threads);
    printf("Number of runs: %d\n\n", config.num_runs);

    pthread_t threads[MAX_THREADS];
    ThreadData td[MAX_THREADS];
    int chunk = config.array_size / config.num_threads;

    double total_time = 0.0;
    Timer timer;

    for (int run = 0; run < config.num_runs; run++) {
        generate_random_array(arr, config.array_size);

        timer_start(&timer);

        // Create threads for parallel sorting
        for (int i = 0; i < config.num_threads; i++) {
            td[i].arr = arr;
            td[i].left = i * chunk;
            td[i].right = (i == config.num_threads - 1) ? (config.array_size - 1) : ((i + 1) * chunk - 1);
            pthread_create(&threads[i], NULL, threaded_quicksort, &td[i]);
        }

        // Wait for all threads to complete
        for (int i = 0; i < config.num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // Merge sorted chunks
        int current_chunk = chunk;
        while (current_chunk < config.array_size) {
            for (int i = 0; i < config.array_size; i += 2 * current_chunk) {
                int mid = i + current_chunk - 1;
                int right = ((i + 2 * current_chunk - 1) < config.array_size) ? 
                           (i + 2 * current_chunk - 1) : (config.array_size - 1);
                if (mid < right)
                    merge(arr, i, mid, right);
            }
            current_chunk *= 2;
        }

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
        save_benchmark_csv(config.output_file, "Pthreads", config.array_size, config.num_threads, avg_time, 0.0);
    }

    free_array(arr);
    return 0;
}
