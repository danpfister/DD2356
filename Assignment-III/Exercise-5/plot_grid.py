import pandas as pd, matplotlib.pyplot as plt, sys
csv = sys.argv[1] if len(sys.argv) > 1 else "grid_scaling.csv"
d = pd.read_csv(csv)                    
plt.plot(d["Grid"], d["AverageTime"], "o-")
plt.xlabel("Grid size N"); plt.ylabel("time [s]")
plt.tight_layout()
plt.savefig("grid_scaling.png", dpi=150)
plt.show()
