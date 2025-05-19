#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000  // Matrix rows
#define M 1000  // Matrix columns

void initialize_matrix(double matrix[N][M]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrix[i][j] = i + j * 0.01;
        }
    }
}

void compute_row_sums(double local_matrix[N][M], double local_row_sums[N], int rows_per_proc) {
    for (int i = 0; i < rows_per_proc; i++) {
        local_row_sums[i] = 0.0;
        for (int j = 0; j < M; j++) {
            local_row_sums[i] += local_matrix[i][j];
        }
    }
}

void write_output(double row_sums[N], int size) {
    FILE *f = fopen("row_sums_output_500X1000.txt", "w");
    for (int i = 0; i < size; i++) {
        fprintf(f, "%f\n", row_sums[i]);
    }
    fclose(f);
}

void add_local_totals(double local_row_sums[N], int rows_per_proc, double *local_total) {
    *local_total = 0.0;
    for (int i = 0; i < rows_per_proc; i++) {
        *local_total += local_row_sums[i];
    }
}

int main(int argc, char** argv) {
    double start_time, end_time, compute_start, compute_end, total_time, compute_time;
    int rank, size;
    double matrix[N][M], local_matrix[N][M], row_sums[N], local_row_sums[N], total_sum;

    MPI_Init(&argc, &argv);
    start_time = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = N/size;

    if (rank == 0) {
        initialize_matrix(matrix);
        printf("Total number of processes - %d \n", size);
    }

    // Scatter rows of the matrix to all processes
    MPI_Scatter(matrix, rows_per_proc * M, MPI_DOUBLE, local_matrix, rows_per_proc * M, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    compute_start = MPI_Wtime();
    // Compute local row sums
    compute_row_sums(local_matrix, local_row_sums, rows_per_proc);
    compute_end = MPI_Wtime();

    // Gather row sums from all processes
    MPI_Gather(local_row_sums, rows_per_proc, MPI_DOUBLE, row_sums, rows_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Compute local total sum and reduce to get the global total
    double local_total;
    add_local_totals(local_row_sums, rows_per_proc, &local_total);
    compute_time = compute_end - compute_start;
    MPI_Reduce(&local_total, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total sum of matrix elements: %f\n", total_sum);
        write_output(row_sums, N);
    }

    if (rank == 0) {
        end_time = MPI_Wtime();
        total_time = end_time - start_time;
        printf("Total execution time: %f seconds \n", total_time);
        printf("Computation time: %f seconds \n", compute_time);
    }
    MPI_Finalize();
    return 0;
}
