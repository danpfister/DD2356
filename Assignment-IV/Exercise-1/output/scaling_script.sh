#!/bin/bash

set -euo pipefail

# --- user-tweakable section ---------------------------------------------------
HALO_EXE="../halo"                    # path to the compiled binary
procs=(1 2 4 8 16 32 64 128)      # process counts to benchmark
outfile="strong_scaling.dat"          # where results are stored
# -----------------------------------------------------------------------------


echo "# nprocs  time_seconds" > "${outfile}"

for p in "${procs[@]}"; do
    echo -e "\n=== ${p} tasks ==="
    # Run halo, capture its stdout, grep the timing line, pull out the number
    t=$(srun --ntasks=${p} "${HALO_EXE}" \
         | tee /dev/stderr \
         | awk '/Total time:/ {print $(NF-1)}')

    echo "${p}  ${t}" >> "${outfile}"
done

echo -e "\nDone. Results written to ${outfile}"
