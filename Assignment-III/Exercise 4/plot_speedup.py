import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
df = pd.read_csv("./speedup_results.csv")

# Plot speedup
plt.figure(figsize=(8, 5))
plt.plot(df["Threads"], df["Speedup"], marker='o', linewidth=2)
plt.title("DFTW Speedup vs. Number of Threads")
plt.xlabel("Number of Threads")
plt.ylabel("Speedup (T1 / Tn)")
plt.xticks(df["Threads"])
plt.grid(True)
plt.tight_layout()
plt.savefig("speedup_plot.png")
plt.show()
