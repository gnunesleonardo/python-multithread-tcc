#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define MAX_ITER 100000
#define MAX_TOL 0.0001
#define INITIAL_ERR 1000000.0

void init_matrix(double **a, double **a_new, int max_m, int max_n)
{
    for (int i = 0; i < max_m; i++)
    {
        for (int j = 0; j < max_n; j++)
        {
            a[i][j] = 30.0;
            a_new[i][j] = 30.0;
        }
    }

    for (int i = 0; i < max_m; i++)
    {
        a[i][0] = 10.0;
        a[i][max_n - 1] = 140.0;
        a_new[i][0] = 10.0;
        a_new[i][max_n - 1] = 140.0;
    }

    for (int j = 0; j < max_n; j++)
    {
        a[0][j] = 20.0;
        a[max_m - 1][j] = 100.0;
        a_new[0][j] = 20.0;
        a_new[max_m - 1][j] = 100.0;
    }
}

int main(int argc, char *argv[])
{
    struct timeval start_time, end_time;
    long total_time, seconds, micros;

    int n_threads_array[] = { 1, 2, 4, 8, 16, 28 };
    int n_threads_array_len = (int)(sizeof(n_threads_array) / sizeof(n_threads_array[0]));

    int n_matrix_array[] = { 512, 1024, 2048 };
    int n_matrix_array_len = (int)(sizeof(n_matrix_array) / sizeof(n_matrix_array[0]));

    int max_m, max_n, i, j, iter;
    double **a, **a_new, err;

    for (int thr_index = 0; thr_index < n_threads_array_len; thr_index++)
    {
        int n_threads = n_threads_array[thr_index];
        omp_set_num_threads(n_threads);

        for (int matrix_index = 0; matrix_index < n_matrix_array_len; matrix_index++)
        {
            max_m = max_n = n_matrix_array[matrix_index];
            err = INITIAL_ERR;
            iter = 0;
            
            a = (double **)malloc(max_m * sizeof(double *));
            a_new = (double **)malloc(max_m * sizeof(double *));
            for (int i = 0; i < max_m; i++)
            {
                a[i] = (double *)malloc(max_n * sizeof(double));
                a_new[i] = (double *)malloc(max_n * sizeof(double));
            }
            init_matrix(a, a_new, max_m, max_n);

            gettimeofday(&start_time, NULL);
            #pragma omp parallel shared(a, a_new, err, iter) private(i, j)
            {
                while (err > MAX_TOL && iter < MAX_ITER)
                {
                    #pragma omp barrier
                    #pragma omp single
                    {
                        err = 0.0;
                    }

                    #pragma omp for reduction(max: err)
                    for (int i = 1; i < max_m - 1; i++)
                    {
                        for (int j = 1; j < max_n - 1; j++)
                        {
                            a_new[i][j] = 0.25 * (a[i][j + 1] + a[i][j - 1] + a[i - 1][j] + a[i + 1][j]);
                            err = fmax(err, fabs(a_new[i][j] - a[i][j]));
                        }
                    }

                    #pragma omp for
                    for (int i = 1; i < max_n - 1; i++)
                    {
                        for (int j = 1; j < max_m - 1; j++)
                        {
                            a[i][j] = a_new[i][j];
                        }
                    }

                    #pragma omp single
                    {
                        iter++;
                    }
                }
            }
            gettimeofday(&end_time, NULL);

            seconds = (end_time.tv_sec - start_time.tv_sec);
            micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
            printf("[%d][%d] threads | %dx%d: %d.%ds\n", iter, n_threads, max_m, max_n, seconds, micros);

            free(a);
            free(a_new);
        }
        printf("\n-------------------------\n\n");
    }

    return 1;
}