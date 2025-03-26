#!/bin/bash -l

#SBATCH -J mpi_hello_world
#SBATCH -t 1:00:00
#SBATCH -A edu25.DD2356
#SBATCH -p shared
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=2
#SBATCH --nodes=1

# Run the executable file
srun -n 128 ./a.out > output