#!/bin/bash -l

#SBATCH -J spmv
#SBATCH -t 1:00:00
#SBATCH -A edu25.DD2356
#SBATCH -p shared
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=2
#SBATCH --nodes=1
#SBATCH -e error_file.e

# Run the executable file
for arg in 10 100 1000 10000; do
    srun -n 1 ./spmv.out "$arg"
done