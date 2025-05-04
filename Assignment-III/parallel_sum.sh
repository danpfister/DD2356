#!/bin/bash -l

#SBATCH -J parallel_sum_array
#SBATCH --output=logs/sum_benchmark_%A_%a.out
#SBATCH --error=logs/sum_benchmark_%A_%a.err
#SBATCH -t 1:00:00
#SBATCH -A edu25.DD2356
#SBATCH -p shared
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=128
#SBATCH --nodes=1
#SBATCH --array=0-36

REPS=10  # number of repetitions per run

# Define mode and thread combinations
MODES=(0 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 3 3 3 3 4 4 4 4)
THREADS=(1 1 2 4 8 16 20 24 28 32 1 2 4 8 16 20 24 28 32 1 32 64 128 1 32 64 128)

# Get mode and thread count for this task
MODE=${MODES[$SLURM_ARRAY_TASK_ID]}
THREAD=${THREADS[$SLURM_ARRAY_TASK_ID]}

echo "Running mode=$MODE with $THREAD threads for $REPS repetitions..."
./parallel_sum $MODE $THREAD $REPS > outputs2/mode_${MODE}_threads_${THREAD}.txt