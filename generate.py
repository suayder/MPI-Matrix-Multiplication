import numpy as np

a = np.random.randint(1000, size=(100,100))

np.savetxt('array2.txt', a, fmt="%i")