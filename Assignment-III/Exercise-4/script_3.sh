#!/usr/bin/env bash
set -euo pipefail

EXEC=./DFTW
NUM_RUNS=5
THREADS=(1 32 64 128)
CSVFILE=speedup_results.csv

printf "Threads,AverageTime,Speedup\n" > "$CSVFILE"
echo "Benchmarking: ${THREADS[*]} threads, $NUM_RUNS runs each"

baseline=

for T in "${THREADS[@]}"; do
    export OMP_NUM_THREADS=$T
    tmpfile=$(mktemp)

    for ((i=1; i<=NUM_RUNS; i++)); do
        time=$(srun --cpu-bind=cores "$EXEC" | awk '/DFTW computation/{print $(NF-1)}')
        printf "  Run %d/%d with %3d threads: %s s\n" "$i" "$NUM_RUNS" "$T" "$time"
        printf "%s\n" "$time" >> "$tmpfile"
    done

    avg=$(awk '{s+=$1; n++} END{printf "%.6f", s/n}' "$tmpfile")

    [[ -z $baseline ]] && baseline=$avg
    speedup=$(awk -v b="$baseline" -v a="$avg" 'BEGIN{printf "%.2f", b/a}')

    printf "  ↳ Avg %.6fs | Speed-up ×%s\n\n" "$avg" "$speedup"
    printf "%s,%s,%s\n" "$T" "$avg" "$speedup" >> "$CSVFILE"

    rm "$tmpfile"
done

echo "✅  All done – results saved to $CSVFILE"
