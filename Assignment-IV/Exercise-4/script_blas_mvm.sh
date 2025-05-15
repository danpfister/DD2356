#!/bin/bash -l

#SBATCH -J mvm_blas
#SBATCH --output=blas_mvm.out
#SBATCH -t 1:00:00
#SBATCH -A edu25.DD2356
#SBATCH -p shared
#SBATCH --ntasks=10
#SBATCH --cpus-per-task=2
#SBATCH --nodes=1

srun -n 10 ./blas_mvm
