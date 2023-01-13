from numba import jit
import numpy as np
import time

MAX_M = 100
MAX_N = 100
MAX_ITER = 100000
MAX_TOL = 0.0001
INITIAL_ERR = 1000000.0

@jit()
def compute_stencil(a, a_new, m, n):
    err = 0.0
    for i in range(1, (m - 1)):
        for j in range (1, (n - 1)):
            a_new[i][j] = 0.25 * (a[i][j + 1] + a[i][j - 1] + a[i - 1][j] + a[i + 1][j]);
            err = max(err, abs(a_new[i][j] - a[i][j]))

    for i in range(1, m - 1):
        for j in range(1, n - 1):
            a[i][j] = a_new[i][j]
    
    return err

a = np.zeros((MAX_M, MAX_N))
a_new = np.zeros((MAX_M, MAX_N))
err = INITIAL_ERR
iter = 0;

for i in range(0, MAX_M):
    for j in range (0, MAX_N):
        a[i][j] = 30.0

for i in range(0, MAX_M):
    a[i][0] = 10.0
    a[i][MAX_N - 1] = 140.0

for j in range(0, MAX_N):
    a[0][j] = 20.0
    a[MAX_M - 1][j] = 100.0

start_time = time.time()
while (err > MAX_TOL and iter < MAX_ITER):
    err = compute_stencil(a, a_new, MAX_M, MAX_N)
    iter += 1

end_time = (time.time() - start_time);
print("[%d]: %ss" % (iter, end_time))
