import numpy as np
import matplotlib.pyplot as plt
import glob
import os
from collections import defaultdict

# Step 1: Match all per-rank files
ranked_files = glob.glob("output/wave_output_rank*_step*.txt")

# Step 2: Group files by step number
step_groups = defaultdict(list)
for file in ranked_files:
    parts = file.split('_')
    rank = int(parts[2][4:])  # 'rankX' → X
    step = int(parts[3].split('.')[0][4:])  # 'stepY.txt' → Y
    step_groups[step].append((rank, file))

# Step 3: For each step, reconstruct full data
for step in sorted(step_groups.keys()):
    rank_files = sorted(step_groups[step])  # sort by rank
    full_data = []
    for rank, file in rank_files:
        data = np.loadtxt(file)
        full_data.extend(data)
        os.remove(file)  # Step 5: clean up

    full_data = np.array(full_data)

    # Step 4: Write to wave_output_<step>.txt
    out_filename = f"output/wave_output_{step}.txt"
    np.savetxt(out_filename, full_data)

# Step 6: Continue with the original plotting logic
files = sorted(glob.glob("output/wave_output_*.txt"))

for file in files:
    data = np.loadtxt(file)
    step = file.split('_')[-1].split('.')[0]
    plt.plot(data, label=f"Time step {step}")

plt.xlabel("Grid Point")
plt.ylabel("Wave Amplitude")
plt.title("Wave Equation Evolution")
plt.legend()
plt.show()
