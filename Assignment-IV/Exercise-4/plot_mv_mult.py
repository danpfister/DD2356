import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("blas_output.txt")
plt.plot(data, marker='o', label='Matrix-Vector Product')
plt.xlabel("Index")
plt.ylabel("Value")
plt.title("BLAS Matrix-Vector Multiplication Output")
plt.legend()
plt.savefig('blas_output.png', bbox_inches='tight')
plt.show()