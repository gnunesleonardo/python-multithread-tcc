#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define MAX_M 100
#define MAX_N 100
#define MAX_ITER 100000
#define MAX_TOL 0.0001
#define INITIAL_ERR 1000000.0

void init_matrix(double **a, double **a_new)
{
    for (int i = 0; i < MAX_M; i++)
    {
        for (int j = 0; j < MAX_N; j++)
        {
            a[i][j] = 30.0;
            a_new[i][j] = 30.0;
        }
    }

    for (int i = 0; i < MAX_M; i++)
    {
        a[i][0] = 10.0;
        a[i][MAX_N - 1] = 140.0;
        a_new[i][0] = 10.0;
        a_new[i][MAX_N - 1] = 140.0;
    }

    for (int j = 0; j < MAX_N; j++)
    {
        a[0][j] = 20.0;
        a[MAX_M - 1][j] = 100.0;
        a_new[0][j] = 20.0;
        a_new[MAX_M - 1][j] = 100.0;
    }
}

int main(int argc, char *argv[])
{
	struct timeval start_time, end_time;
    long total_time;

    int i, j, iter = 0;
    double err = INITIAL_ERR;

    double **a = (double **)malloc(MAX_M * sizeof(double*));
    double **a_new = (double **)malloc(MAX_M * sizeof(double*));
    for(int i = 0; i < MAX_M; i++) {
        a[i] = (double *)malloc(MAX_N * sizeof(double));
        a_new[i] = (double *)malloc(MAX_N * sizeof(double));
    }

	init_matrix(a, a_new);

	gettimeofday(&start_time, NULL);
	while (err > MAX_TOL && iter < MAX_ITER)
	{
		err = 0.0;
		for (int i = 1; i < MAX_M - 1; i++)
		{
			for (int j = 1; j < MAX_N - 1; j++)
			{
				a_new[i][j] = 0.25 * (a[i][j + 1] + a[i][j - 1] + a[i - 1][j] + a[i + 1][j]);
				err = fmax(err, fabs(a_new[i][j] - a[i][j]));
			}
		}

		for (int i = 1; i < MAX_M - 1; i++)
		{
			for (int j = 1; j < MAX_N - 1; j++)
			{
				a[i][j] = a_new[i][j];
			}
		}

		iter++;
	}
	gettimeofday(&end_time, NULL);

	long seconds = (end_time.tv_sec - start_time.tv_sec);
	long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
	printf("[%d]: %d.%ds\n", iter, seconds, micros);

	free(a);
    free(a_new);
    return 1;
}