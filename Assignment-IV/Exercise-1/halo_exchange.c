#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 16777216           // Global grid size
#define STEPS 1000
#define C 1.0
#define DT 0.01
#define DX 1.0

#define MAX_LOCAL_N 16777216 // Adjust to match N / min_nprocs (which is 1 procs)
double u[MAX_LOCAL_N + 2];
double u_prev[MAX_LOCAL_N + 2];
double u_next[MAX_LOCAL_N + 2];

void initialize(int local_N, int rank) {
    for (int i = 1; i <= local_N; i++) {
        int global_idx = rank * local_N + (i - 1);
        u[i] = sin(2.0 * M_PI * global_idx / N);
        u_prev[i] = u[i];
    }
}

void exchange_halos(int local_N, int rank, int nprocs) {
    int left = rank - 1;
    int right = rank + 1;

    // If not the last process, sendrecv to right
    if (rank != nprocs - 1) {
        MPI_Sendrecv(&u[local_N], 1, MPI_DOUBLE, right, 1,
                     &u[local_N + 1], 1, MPI_DOUBLE, right, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // If not the first process, sendrecv to left
    if (rank != 0) {
        MPI_Sendrecv(&u[1], 1, MPI_DOUBLE, left, 0,
                     &u[0], 1, MPI_DOUBLE, left, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}


void compute_step(int local_N) {
    for (int i = 1; i <= local_N; i++) {
        u_next[i] = 2.0 * u[i] - u_prev[i] + 
                    C * C * DT * DT / (DX * DX) * 
                    (u[i + 1] - 2.0 * u[i] + u[i - 1]);
    }

    for (int i = 1; i <= local_N; i++) {
        u_prev[i] = u[i];
        u[i] = u_next[i];
    }
}

void write_output(int step, int local_N, int rank) {
    char filename[50];
    sprintf(filename, "wave_output_rank%d_step%d.txt", rank, step);
    FILE *f = fopen(filename, "w");
    for (int i = 1; i <= local_N; i++) {
        fprintf(f, "%f\n", u[i]);
    }
    fclose(f);
}

int main(int argc, char** argv) {
    int rank, nprocs;
    double t_start, t_end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //printf("process count : %d\n", nprocs);

    // We could've made the code resilient to this, but we'll just continue this way for the lab
    if (N % nprocs != 0) {
        if (rank == 0)
            fprintf(stderr, "N must be divisible by number of processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int local_N = N / nprocs;
    if (local_N + 2 > MAX_LOCAL_N + 2) {
        if (rank == 0)
            fprintf(stderr, "Increase MAX_LOCAL_N.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    initialize(local_N, rank);

    MPI_Barrier(MPI_COMM_WORLD); // timing sync, everybody stops until caught up
    t_start = MPI_Wtime();

    for (int step = 0; step < STEPS; step++) {
        exchange_halos(local_N, rank, nprocs);
        compute_step(local_N);
        /*if (step % 10 == 0)
            write_output(step, local_N, rank);*/
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t_end = MPI_Wtime();

    if (rank == 0) {
        printf("Simulation complete.\n");
        printf("Total time: %f seconds\n", t_end - t_start);
    }

    MPI_Finalize();
    return 0;
}
