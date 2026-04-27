#include <mpi.h>
#include "../include/quicksort_common.h"

#define SIZE DEFAULT_ARRAY_SIZE

// gcc -I"D:\MSMPI\Include" MpiQuickSort.c -L "D:\MSMPI\Lib\x64" -lmsmpi -o MpiQuickSort.exe
// mpiexec -n 2 MpiQuickSort.exe



void quicksort(int *arr, int left, int right) {
    if (left < right) {
        int pivot = arr[right];
        int i = left - 1;
        for (int j = left; j < right; j++) {
            if (arr[j] <= pivot) {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        swap(&arr[i + 1], &arr[right]);
        int pi = i + 1;
        quicksort(arr, left, pi - 1);
        quicksort(arr, pi + 1, right);
    }
}

void merge(int *a, int *b, int n, int m, int *result) {
    int i = 0, j = 0, k = 0;
    while (i < n && j < m) {
        if (a[i] <= b[j]) {
            result[k++] = a[i++];
        }
        else {
            result[k++] = b[j++];
        }
    }
    while (i < n) {
        result[k++] = a[i++];
    }
    while (j < m) {
        result[k++] = b[j++];
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int *data = NULL;
    int *local_data = NULL;
    int *counts = NULL;
    int *displs = NULL;
    int local_n;
    int array_size = SIZE;

    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parse command line arguments (only on rank 0)
    if (rank == 0) {
        BenchmarkConfig config = parse_arguments(argc, argv);
        array_size = config.array_size;
        
        printf("MPI QuickSort Benchmark\n");
        printf("Array size: %d elements\n", array_size);
        printf("Number of processes: %d\n", size);
        printf("Number of runs: %d\n\n", config.num_runs);
    }

    // Broadcast array size to all processes
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate data distribution
    if (rank == 0) {
        data = allocate_array(array_size);
        generate_random_array(data, array_size);

        counts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));
        int rem = array_size % size;
        int sum = 0;
        for (int i = 0; i < size; i++) {
            counts[i] = array_size / size + (i < rem ? 1 : 0);
            displs[i] = sum;
            sum += counts[i];
        }

        start_time = MPI_Wtime();
    }

    if (rank != 0) {
        counts = (int *)malloc(size * sizeof(int));
    }
    MPI_Bcast(counts, size, MPI_INT, 0, MPI_COMM_WORLD);
    local_n = counts[rank];
    local_data = (int *)malloc(local_n * sizeof(int));

    if (rank != 0)
        displs = (int *)malloc(size * sizeof(int));
    MPI_Bcast(displs, size, MPI_INT, 0, MPI_COMM_WORLD);

    // Distribute data to all processes
    MPI_Scatterv(data, counts, displs, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Local quicksort
    quicksort(local_data, 0, local_n - 1);

    // Merge results step by step
    int step = 1;
    while (step < size) {
        if (rank % (2 * step) == 0) {
            if (rank + step < size) {
                int recv_size;
                MPI_Recv(&recv_size, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int *recv_data = (int *)malloc(recv_size * sizeof(int));
                MPI_Recv(recv_data, recv_size, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *merged = (int *)malloc((local_n + recv_size) * sizeof(int));
                merge(local_data, recv_data, local_n, recv_size, merged);

                free(local_data);
                free(recv_data);
                local_data = merged;
                local_n += recv_size;
            }
        } else {
            int target = rank - step;
            MPI_Send(&local_n, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
            MPI_Send(local_data, local_n, MPI_INT, target, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    if (rank == 0) {
        end_time = MPI_Wtime();
        
        // Verify result
        if (!verify_sorted(local_data, local_n, 1)) {
            printf("Error: Sort failed verification!\n");
        } else {
            printf("Results:\n");
            printf("Time: %.6f seconds\n", end_time - start_time);
            printf("Throughput: %.2f M elements/second\n", array_size / ((end_time - start_time) * 1000000));
        }

        free(data);
        free(counts);
        free(displs);
        free(local_data);
    } else {
        free(counts);
        free(displs);
        free(local_data);
    }

    MPI_Finalize();
    return 0;
}

