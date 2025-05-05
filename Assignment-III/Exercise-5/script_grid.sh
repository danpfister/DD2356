set -euo pipefail

THREADS=8               
RUNS=3                  # repetitions per grid size (keep it small => fast demo)
SIZES=(100 200 400 800 1200 1600)   # grid sizes N to test
CSV=grid_scaling.csv

printf "Grid,AverageTime\n" > "$CSV"

echo "Fixed threads : $THREADS"
echo "Runs per size : $RUNS"
echo "Grid sizes    : ${SIZES[*]}"
echo

for N in "${SIZES[@]}"; do
    EXE=shallow_N${N}

    # (re)compile with -D to override the #define N in the source
    gcc -O3 -fopenmp -DN="$N" -o "$EXE" shallow_water_parallel.c

    export OMP_NUM_THREADS=$THREADS

    sum=0
    for ((r=1; r<=RUNS; r++)); do
        t=$(./"$EXE" | awk '/Computation completed/{print $(NF-1)}')
        printf "N=%-5d run %2d/%d : %ss\n" "$N" "$r" "$RUNS" "$t"
        sum=$(awk -v s="$sum" -v x="$t" 'BEGIN{print s+x}')
    done

    avg=$(awk -v s="$sum" -v n="$RUNS" 'BEGIN{printf "%.6f", s/n}')
    printf "  ↳ average %.6fs\n\n" "$avg"

    printf "%s,%s\n" "$N" "$avg" >> "$CSV"
done

echo "✅  Done – results in $CSV"
