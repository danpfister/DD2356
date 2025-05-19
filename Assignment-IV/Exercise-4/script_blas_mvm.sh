#!/bin/bash -l

#SBATCH -J mvm_blas
#SBATCH --output=blas_mvm.out
#SBATCH -t 1:00:00
#SBATCH -A edu25.DD2356
#SBATCH -p shared
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=2
#SBATCH --nodes=1

for N in 1000 2000 4000 5000 8000 10000 16000; do
    srun -n 8 ./blas_mvm $N
done

for p in 1 2 4 8 16 32; do
    srun -n $p ./blas_mvm 4000
done
