from numba import jit, prange, set_num_threads
import numpy as np
import time

MAX_ITER = 100000
MAX_TOL = 0.0001
INITIAL_ERR = 1000000.0

@jit(parallel=True, nogil=True, cache=True)
def compute_stencil(a, a_new, m, n):
    err = 0.0
    for i in prange(1, (m - 1)):
        for j in range (1, (n - 1)):
            a_new[i][j] = 0.25 * (a[i][j + 1] + a[i][j - 1] + a[i - 1][j] + a[i + 1][j]);
            err = max(err, abs(a_new[i][j] - a[i][j]))

    for i in prange(1, m - 1):
        for j in range(1, n - 1):
            a[i][j] = a_new[i][j]
    
    return err

n_threads_array = [1, 2, 4, 8, 16, 28]
n_matrix_array = [512, 1024, 2048]

for n_thread in n_threads_array:
    set_num_threads(n_thread)

    for n_matrix in n_matrix_array:
        max_m = max_n = n_matrix
        err = INITIAL_ERR
        iter = 0;

        a = np.zeros((max_m, max_n))
        a_new = np.zeros((max_m, max_n))
        
        for i in range(0, max_m):
            for j in range (0, max_n):
                a[i][j] = 30.0

        for i in range(0, max_m):
            a[i][0] = 10.0
            a[i][max_n - 1] = 140.0

        for j in range(0, max_n):
            a[0][j] = 20.0
            a[max_m - 1][j] = 100.0

        start_time = time.time()
        while (err > MAX_TOL and iter < MAX_ITER):
            err = compute_stencil(a, a_new, max_m, max_n)
            iter += 1

        end_time = (time.time() - start_time)
        print("[%d][%d] threads | %dx%d: %ss\n" % (iter, n_thread, max_m, max_n, end_time))

    print("-------------------------");
    print()
