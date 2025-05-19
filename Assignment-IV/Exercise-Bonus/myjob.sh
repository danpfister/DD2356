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
# and write the output into my_output_file
srun -n 1 ./bonus_ser > bonus_ser.txt
srun -n 1 ./bonus_prl > bonus_prl_1.txt
srun -n 2 ./bonus_prl > bonus_prl_2.txt
srun -n 5 ./bonus_prl > bonus_prl_5.txt
srun -n 10 ./bonus_prl > bonus_prl_10.txt
srun -n 20 ./bonus_prl > bonus_prl_20.txt

