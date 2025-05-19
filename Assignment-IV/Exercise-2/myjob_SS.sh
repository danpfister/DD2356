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
#SBATCH --nodes=2
#SBATCH -e error_file.e

# Run the executable file 
# and write the output into my_output_file
srun -n 1 --mem=64G ./sum_1000 > sum_1000_1.txt
srun -n 2 --mem=64G ./sum_1000 > sum_1000_2.txt
srun -n 5 --mem=64G ./sum_1000 > sum_1000_5.txt
srun -n 10 --mem=64G ./sum_1000 > sum_1000_10.txt
srun -n 20 --mem=64G ./sum_1000 > sum_1000_20.txt
srun -n 50 --mem=64G ./sum_1000 > sum_1000_50.txt
srun -n 100 --mem=64G ./sum_1000 > sum_1000_100.txt
