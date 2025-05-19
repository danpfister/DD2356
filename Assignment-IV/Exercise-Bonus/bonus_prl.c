#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 20  // Grid size
#define STEPS 100  // Simulation steps

void initialize(int grid[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = rand() % 2;
}

int count_neighbors(int x, int y, int rows, int grid[rows][N]) {
    int sum = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y) continue;
            int ni = (i + rows) % rows;
            int nj = (j + N) % N;
            sum += grid[ni][nj];
        }
    }
    return sum;
}

void exchange_ghost_rows(int rank, int size, int rows_per_proc, int local_grid[rows_per_proc+2][N]) {
    MPI_Request reqs[4];
    int up = (rank - 1 + size) % size;
    int down = (rank + 1) % size;

    MPI_Isend(local_grid[rows_per_proc], N, MPI_INT, down, 0, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(local_grid[0], N, MPI_INT, up, 0, MPI_COMM_WORLD, &reqs[1]);

    MPI_Isend(local_grid[1], N, MPI_INT, up, 1, MPI_COMM_WORLD, &reqs[2]);
    MPI_Irecv(local_grid[rows_per_proc + 1], N, MPI_INT, down, 1, MPI_COMM_WORLD, &reqs[3]);

    MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
}

void update_grid(int rows_per_proc, int local_grid[rows_per_proc+2][N], int new_grid[rows_per_proc+2][N]) {
    for (int i = 1; i <= rows_per_proc; i++) {
        for (int j = 0; j < N; j++) {
            int neighbors = count_neighbors(i, j, rows_per_proc + 2, local_grid);
            if (local_grid[i][j] == 1 && (neighbors < 2 || neighbors > 3))
                new_grid[i][j] = 0;
            else if (local_grid[i][j] == 0 && neighbors == 3)
                new_grid[i][j] = 1;
            else
                new_grid[i][j] = local_grid[i][j];
        }
    }
    for (int i = 1; i <= rows_per_proc; i++)
        for (int j = 0; j < N; j++)
            local_grid[i][j] = new_grid[i][j];
}

void write_full_grid(int step, int grid[N][N]) {
    char filename[64];
    sprintf(filename, "gol_output_prl_step%d.txt", step);
    FILE *f = fopen(filename, "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%d ", grid[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0)
            printf("Grid size N=%d must be divisible by number of processes=%d\n", N, size);
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = (N+size-1)/ size;
    int local_grid[rows_per_proc + 2][N];
    int new_grid[rows_per_proc + 2][N];
    int full_grid[N][N];

    if (rank == 0) {
        initialize(full_grid);
    }

    MPI_Scatter(&full_grid[0][0], rows_per_proc * N, MPI_INT,
                &local_grid[1][0], rows_per_proc * N, MPI_INT,
                0, MPI_COMM_WORLD);

    // Initialize ghost rows
    for (int j = 0; j < N; j++) {
        local_grid[0][j] = 0;
        local_grid[rows_per_proc + 1][j] = 0;
    }

    double start_time = MPI_Wtime();

    for (int step = 0; step < STEPS; step++) {
        exchange_ghost_rows(rank, size, rows_per_proc, local_grid);
        update_grid(rows_per_proc, local_grid, new_grid);

        if (step % 10 == 0) {
            MPI_Gather(&local_grid[1][0], rows_per_proc * N, MPI_INT,
                       &full_grid[0][0], rows_per_proc * N, MPI_INT,
                       0, MPI_COMM_WORLD);

            if (rank == 0) {
                write_full_grid(step, full_grid);
            }
        }
    }

    MPI_Gather(&local_grid[1][0], rows_per_proc * N, MPI_INT,
               &full_grid[0][0], rows_per_proc * N, MPI_INT,
               0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Execution time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
