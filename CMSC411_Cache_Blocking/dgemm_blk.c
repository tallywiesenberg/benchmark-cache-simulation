/* simple 2D blocked Matrix Multiply  */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

// #define BLOCKSIZE 32

#define GET_TIME(now)                           \
    {                                           \
        struct timeval t;                       \
        gettimeofday(&t, NULL);                 \
        now = t.tv_sec + t.tv_usec / 1000000.0; \
    }

int block_size;

void manipulate_matrix(double *, int, int);
void print_matrix(double *, int, int);
void dgemm(int, double *, double *, double *);
void do_block(int, int, int, int, double *, double *, double *);

int main(int argc, char **argv)
{
    int n = atoi(argv[1]);
    block_size = atoi(argv[2]); // changed block size from global to CLI argument for easier manipulation

    int cols, rows;
    clock_t cstart, cend;
    double start, end, el1;
    double cpu_time_used;

    // square nonnegate matrices
    cols = n;
    rows = n;

    /* create 2D array with dimensions determined at runtime */
    double *matrix = malloc(cols * rows * sizeof(double));
    double *matA = malloc(cols * rows * sizeof(double));
    double *matB = malloc(cols * rows * sizeof(double));
    double *matC = malloc(cols * rows * sizeof(double));

    /* initialise it (for the sake of illustration we want 1.0 on the diagonal) */
    int x, y;
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            if (x == y)
                matrix[y * cols + x] = 1.0;
            else
                matrix[y * cols + x] = 4.0;
        }
    }

    manipulate_matrix(matrix, cols, rows);
    manipulate_matrix(matA, cols, rows);
    manipulate_matrix(matB, cols, rows);
    manipulate_matrix(matC, cols, rows);

    GET_TIME(start);
    cstart = clock();

    dgemm(rows, matA, matB, matC);

    cend = clock();
    GET_TIME(end);

    if (n < 10) // only print if matrix is small, for checking
        // print_matrix(matrix, cols, rows);

    free(matrix);
    free(matA);
    free(matB);
    free(matC);

    el1 = end - start;
    cpu_time_used = ((double)(cend - cstart)) / CLOCKS_PER_SEC;
    printf("%d,%d,%f", block_size, n, el1); // made edits to printed data to fit csv form

    return 0;
}

void print_matrix(double *matrix, int cols, int rows)
{
    int x, y;
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            printf("%f ", matrix[y * cols + x]);
        }
        printf("\n");
    }
}

/* do something with each element in the matrix, e.g. scale by 2 */
void manipulate_matrix(double *matrix, int cols, int rows)
{
    int x, y;

    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            matrix[y * cols + x] *= random();
        }
    }
}

void do_block(int n, int si, int sj, int sk, double *A, double *B, double *C)
{
    for (int i = si; i < si + block_size; ++i)
        for (int j = sj; j < sj + block_size; ++j)
        {
            double cij = C[i + j * n]; /* cij = C[i][j] */
            for (int k = sk; k < sk + block_size; k++)
                cij += A[i + k * n] * B[k + j * n]; /* cij+=A[i][k]*B[k][j] */
            C[i + j * n] = cij;                     /* C[i][j] = cij */
        }
}
void dgemm(int n, double *A, double *B, double *C)
{
    for (int sj = 0; sj < n; sj += block_size)
        for (int si = 0; si < n; si += block_size)
            for (int sk = 0; sk < n; sk += block_size)
                do_block(n, si, sj, sk, A, B, C);
}
