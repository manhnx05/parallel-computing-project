#include "../include/quicksort_common.h"

#define N DEFAULT_ARRAY_SIZE

// Phân hoạch tăng dần
int partition_asc(int arr[], int left, int right) {
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

// Phân hoạch giảm dần
int partition_desc(int arr[], int left, int right) {
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[j] > pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[right]);
    return i + 1;
}

// Quicksort tăng dần
void quicksort_asc(int arr[], int left, int right) {
    if (left < right) {
        int pivot = partition_asc(arr, left, right);
        quicksort_asc(arr, left, pivot - 1);
        quicksort_asc(arr, pivot + 1, right);
    }
}

// Quicksort giảm dần
void quicksort_desc(int arr[], int left, int right) {
    if (left < right) {
        int pivot = partition_desc(arr, left, right);
        quicksort_desc(arr, left, pivot - 1);
        quicksort_desc(arr, pivot + 1, right);
    }
}

void generate_random_array(int arr[], int n) {
    for (int i = 0; i < n; i++)
        arr[i] = rand();
}

void copy_array(const int src[], int dest[], int n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}

int main(int argc, char* argv[]) {
    BenchmarkConfig config = parse_arguments(argc, argv);
    
    int* arr = allocate_array(config.array_size);
    int* arr_copy = allocate_array(config.array_size);
    
    printf("Sequential QuickSort Benchmark\n");
    printf("Array size: %d elements\n", config.array_size);
    printf("Number of runs: %d\n\n", config.num_runs);

    double total_time_asc = 0.0, total_time_desc = 0.0;
    Timer timer;

    for (int run = 0; run < config.num_runs; run++) {
        generate_random_array(arr, config.array_size);
        copy_array(arr, arr_copy, config.array_size);

        // Sắp xếp tăng dần
        timer_start(&timer);
        quicksort_asc(arr, 0, config.array_size - 1);
        timer_end(&timer);
        double time_asc = get_elapsed_time(&timer);
        total_time_asc += time_asc;

        if (config.verify_result && !verify_sorted(arr, config.array_size, 1)) {
            printf("Error: Ascending sort failed verification!\n");
            return 1;
        }

        // Sắp xếp giảm dần
        timer_start(&timer);
        quicksort_desc(arr_copy, 0, config.array_size - 1);
        timer_end(&timer);
        double time_desc = get_elapsed_time(&timer);
        total_time_desc += time_desc;

        if (config.verify_result && !verify_sorted(arr_copy, config.array_size, 0)) {
            printf("Error: Descending sort failed verification!\n");
            return 1;
        }

        if (config.num_runs > 1) {
            printf("Run %d - Ascending: %.4f s, Descending: %.4f s\n", 
                   run + 1, time_asc, time_desc);
        }
    }

    double avg_time_asc = total_time_asc / config.num_runs;
    double avg_time_desc = total_time_desc / config.num_runs;
    double avg_time = (avg_time_asc + avg_time_desc) / 2.0;

    printf("\nResults:\n");
    printf("Average ascending sort time: %.4f seconds\n", avg_time_asc);
    printf("Average descending sort time: %.4f seconds\n", avg_time_desc);
    printf("Overall average time: %.4f seconds\n", avg_time);

    if (config.output_csv) {
        save_benchmark_csv(config.output_file, "Sequential", config.array_size, 1, avg_time, 1.0);
    }

    free_array(arr);
    free_array(arr_copy);
    return 0;
}
