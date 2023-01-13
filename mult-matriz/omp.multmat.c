#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define MAX_M 10000

int main()
{
    struct timeval start_time, end_time;
    long total_time;
    int i, j, k;
    int **ma, **mb, **mc;

    int n_threads_array[] = { 1, 2, 4 };
    int n_threads_array_len = (int)(sizeof(n_threads_array) / sizeof(n_threads_array[0]));
    
    for (int thr_index = 0; thr_index < n_threads_array_len; thr_index++)
    {
        int n_threads = n_threads_array[thr_index];
        omp_set_num_threads(n_threads);

        ma = (int **)malloc(MAX_M * sizeof(int *));
        mb = (int **)malloc(MAX_M * sizeof(int *));
        mc = (int **)malloc(MAX_M * sizeof(int *));
        for (int i = 0; i < MAX_M; i++)
        {
            ma[i] = (int *)malloc(MAX_M * sizeof(int));
            mb[i] = (int *)malloc(MAX_M * sizeof(int));
            mc[i] = (int *)malloc(MAX_M * sizeof(int));
        }
        
        for (i = 0; i < MAX_M; i++)
        {
            for (j = 0; j < MAX_M; j++)
            {
                ma[i][j] = rand() % 10;
                mb[i][j] = rand() % 10;
            }
        }

        gettimeofday(&start_time, NULL);
        #pragma omp parallel for private(i, j, k) shared(ma, mb, mc)
        for (i = 0; i < MAX_M; ++i)
        {
            for (k = 0; k < MAX_M; ++k)
            {
                for (j = 0; j < MAX_M; ++j)
                {
                    mc[i][j] += ma[i][k] * mb[k][j];
                }
            }
        }
        gettimeofday(&end_time, NULL);

        long seconds = (end_time.tv_sec - start_time.tv_sec);
        long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
        printf("[%d] threads | %d,%d\n", n_threads, seconds, micros);

        for (i = 0; i < MAX_M ; i ++) {
            free(ma[i]);
            free(mb[i]);
            free(mc[i]);
        }
        free(ma);
        free(mb);
        free(mc);
    }
}