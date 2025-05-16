#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <mpi.h>

#define N 1000  // Matrix size

void initialize(double *matrix, double *vector) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = (double)(i % 100) / 10.0;
    }
    for (int i = 0; i < N; i++) {
        vector[i] = (double)(i % 50) / 5.0;
    }
}

int main(int argc, char* argv[]) {
    int rank, n_ranks, provided, iter, n_rows;
    double start_time, elapsed_time;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    start_time = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

    n_rows = N / n_ranks; // num of rows for each process -> assume is nicely divisible
    iter = rank * n_rows; // start of row for each process

    double *vector = (double*) malloc(N * sizeof(double)); // all processes need alloc for vector
    double *local_matrix = (double*) malloc(n_rows * N * sizeof(double));
    double *local_result = (double*) malloc(n_rows * sizeof(double));
    double* matrix = NULL;
    double *result = NULL;

    if (rank == 0) { // only process 0 needs whole matrix and result
        matrix = (double*) malloc(N * N * sizeof(double));
        result = (double*) malloc(N * sizeof(double));
        initialize(matrix, vector);
    }
    
    // cast vector from process 0 to all others
    MPI_Bcast(vector, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // scatter matrix among all other processes
    MPI_Scatter(matrix, n_rows * N, MPI_DOUBLE, &local_matrix, n_rows * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Perform matrix-vector multiplication using BLAS
    cblas_dgemv(CblasRowMajor, CblasNoTrans, n_rows, N, 1.0, local_matrix, N, vector, 1, 0.0, local_result, 1);

    // blocking operation, so processes are guaranteed to have finished mvm
    MPI_Gather(local_result, n_rows, MPI_DOUBLE, result, n_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Write output to file
        FILE *f = fopen("blas_output.txt", "w");
        for (int i = 0; i < N; i++) {
            fprintf(f, "%f\n", result[i]);
        }
        fclose(f);
    }
    
    free(matrix);
    free(local_matrix);
    free(vector);
    free(result);
    free(local_result);

    elapsed_time = MPI_Wtime() - start_time;

    if (rank == 0) {
        printf("BLAS matrix-vector multiplication complete.\n");
        printf("#Ranks: %d, Execution time: %f\n", n_ranks, elapsed_time);
    }

    MPI_Finalize();
    return 0;
}