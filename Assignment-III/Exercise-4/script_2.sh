#!/bin/bash

# Number of OpenMP threads
export OMP_NUM_THREADS=32

# Number of runs
NUM_RUNS=10

# Executable path
EXEC=./DFTW

# Temporary file for timings
TMPFILE=times.tmp

echo "Running $EXEC with $OMP_NUM_THREADS threads using srun ($NUM_RUNS runs)"
echo "" > $TMPFILE

for i in $(seq 1 $NUM_RUNS); do
    echo -n "Run $i... "
    TIME=$(srun --cpu-bind=cores $EXEC | grep "DFTW computation" | awk '{print $4}')
    echo "$TIME seconds"
    echo "$TIME" >> $TMPFILE
done

# Compute average and standard deviation
awk -v n=$NUM_RUNS '
  {
    sum += $1;
    sumsq += $1*$1;
  }
  END {
    avg = sum/n;
    stddev = sqrt((sumsq - n*avg*avg)/(n));
    printf "\nAverage time: %.6f seconds\n", avg;
    printf "Standard deviation: %.6f seconds\n", stddev;
  }
' $TMPFILE

rm $TMPFILE
