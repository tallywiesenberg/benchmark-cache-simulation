/** playing with the cache


To compile: you need ito include timer.h, assuming it is in the local directory

cc -g -o eviction eviction.c

to measure and simulate cache behavior.
valgrind --tool=cachegrind --cache-sim=yes --D1=65536,8,256 --cachegrind-out-file=d1_64k_8_256 ./eviction

now annotate the output
cg_annotate --auto=yes
**/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// #define MAX 1032

int MAX;

#define GET_TIME(now)                           \
	{                                           \
		struct timeval t;                       \
		gettimeofday(&t, NULL);                 \
		now = t.tv_sec + t.tv_usec / 1000000.0; \
	}

int main(int argc, char **argv)
{
	MAX = atoi(argv[1]);
	clock_t cstart, cend;
	double cpu_time_used;

	double A[MAX][MAX], x[MAX], y[MAX];
	int i, j, k;

	// timer variables
	double start1, end1, el1, start2, end2, el2;

	srand48(time(NULL));

	// initialize
	for (i = 0; i < MAX; i++)
	{
		x[i] = drand48();
		y[i] = 0;
		for (j = 0; j < MAX; j++)
		{
			A[i][j] = drand48();
		}
	}
	GET_TIME(start1);
	cstart = clock();
	// first pair of loops
	for (j = 0; j < MAX; j++)
	{
		for (i = 0; i < MAX; i++)
		{
			y[i] += A[i][j] * x[j];
		}
	}
	cend = clock();
	GET_TIME(end1);
	el1 = end1 - start1;
	cpu_time_used = ((double)(cend - cstart)) / CLOCKS_PER_SEC;
	printf("%f", cpu_time_used);

	return 0;
} // end main
