import numpy as np
import threading
import queue    
import time

MAX_M = 100
MAX_N = 100
MAX_ITER = 100000
MAX_TOL = 0.0001
MAX_THREADS = 16

INITIAL_ERR = 1000000.0
N_THREADS = 1

def compute_stencil(start, end, err_queue, barrier):
    global a
    global a_new
    err = 0.0

    start = 1 if start == 0 else start
    end = (end - 1) if (end == (MAX_M)) else end

    for i in range(start, end):
        for j in range(1, MAX_N - 1):
            a_new[i][j] = 0.25 * (a[i][j + 1] + a[i][j - 1] + a[i - 1][j] + a[i + 1][j])
            err = max(err, abs(a_new[i][j] - a[i][j]))

    barrier.wait()
    for i in range(start, end):
        for j in range(1, MAX_N - 1):
            a[i][j] = a_new[i][j]

    return err_queue.put(err)

while(N_THREADS <= MAX_THREADS):
    err = INITIAL_ERR
    iter = 0;
    threads_array = []
    barrier = threading.Barrier(N_THREADS)

    a = np.zeros((MAX_M, MAX_N))
    a_new = np.zeros((MAX_M, MAX_N))
    array_chunks = np.array_split(range(MAX_M), N_THREADS)
    err_queue = queue.Queue()

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
        err = 0.0

        for chunk in array_chunks:
            start = chunk[0]
            end = ((chunk[len(chunk) - 1]) + 1)
            thread = threading.Thread(target=compute_stencil, args=(start, end, err_queue, barrier))
            threads_array.append(thread)
            thread.start()

        for thread in threads_array:
            thread.join()

        while not err_queue.empty():
            err = max(err, err_queue.get())

        iter += 1

    end_time = (time.time() - start_time)
    print("[%d][%d]: %ss" % (N_THREADS, iter, end_time))
    N_THREADS = N_THREADS*2;