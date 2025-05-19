#!/bin/bash -l
# The -l above is required to get the full environment with modules

# The name of the script is myjob
#SBATCH -J myjob
# Only 30 minutes wall-clock time will be given to this job
#SBATCH -t 02:00:00
#SBATCH -A edu25.DD2356
# Number of nodes
#SBATCH -p shared
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=2
#SBATCH --nodes=1
#SBATCH -e error_file.e

# Run the executable file

srun -n 100 --mem=64G ./ex-2-scorep > sum_1000_100.txt