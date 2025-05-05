#!/usr/bin/env bash

set -euo pipefail

EXEC=./shallow          
NUM_RUNS=5             
THREADS=(1 2 4 8 16 32 64 128)
CSVFILE=shallow_speedup.csv

echo "Benchmarking ${EXEC} …"
echo "Thread counts: ${THREADS[*]}"
echo "Runs per configuration: $NUM_RUNS"
echo

printf "Threads,AverageTime,Speedup\n" > "$CSVFILE"

baseline=

for t in "${THREADS[@]}"; do
    export OMP_NUM_THREADS=$t
    tmpfile=$(mktemp)

    for ((i=1; i<=NUM_RUNS; i++)); do
        time=$(srun --cpu-bind=cores "$EXEC" | awk '/Computation completed/{print $(NF-1)}')
        printf "  Run %d/%d with %3d threads: %s s\n" "$i" "$NUM_RUNS" "$t" "$time"
        printf "%s\n" "$time" >> "$tmpfile"
    done

    avg=$(awk '{s+=$1; n++} END{printf "%.6f", s/n}' "$tmpfile")
    [[ -z $baseline ]] && baseline=$avg
    speedup=$(awk -v b="$baseline" -v a="$avg" 'BEGIN{printf "%.2f", b/a}')

    printf "  ↳ Avg %.6fs | Speed-up ×%s\n\n" "$avg" "$speedup"
    printf "%s,%s,%s\n" "$t" "$avg" "$speedup" >> "$CSVFILE"

    rm "$tmpfile"
done

echo "✅  Done – results saved to $CSVFILE"
