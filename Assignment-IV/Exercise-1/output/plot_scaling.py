import numpy as np
import matplotlib.pyplot as plt

# ------------------------------------------------------------------
# read the two-column results file   (# nprocs   time_seconds)
# ------------------------------------------------------------------
procs, times = np.loadtxt("strong_scaling.dat", skiprows=1, unpack=True)

# ------------------------------------------------------------------
# 1) wall-clock time vs. ranks
# ------------------------------------------------------------------
plt.figure()
plt.plot(procs, times, marker='o')
plt.xlabel("MPI ranks")
plt.ylabel("Wall-time [s]")
plt.title("Strong-scaling: wall-time vs. rank count")
plt.grid(True, linestyle="--", alpha=0.6)
plt.tight_layout()
plt.savefig("walltime_linear.png", dpi=300)
