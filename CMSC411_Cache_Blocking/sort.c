/*cputest.c
 * Tyler Simon
 * Do some CPU intensive work with quicksort
 * Print out usage per core getpinfo
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

double cpu_test(int);

int main(int argc, char **argv)
{
  double comptime;
  int val;
  val = atoi(argv[1]);
  comptime = cpu_test(val);

  return 0;
}

int ascending(const void *a, const void *b)
{
  int int_a = *(const int *)a;
  int int_b = *(const int *)b;

  if (int_a > int_b)
    return 1;
  else if (int_a < int_b)
    return -1;
  else
    return 0;
}

int descending(const void *a, const void *b)
{
  int int_a = *(const int *)a;
  int int_b = *(const int *)b;

  if (int_a < int_b)
    return 1;
  else if (int_a > int_b)
    return -1;
  else
    return 0;
}

double getCurrentTime(void)
{
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return (tval.tv_sec + tval.tv_usec / 1000000.0);
}

double cpu_test(int size)
{
  int i;
  double start_time, end_time, fin_time;
  int *nums;
  srand(time(NULL));

  nums = (int *)malloc(sizeof(int) * size);
  if (!nums)
  {
    printf("ERROR ALLOCING MEM\n");
    exit(1);
  }
  // printf("Populating %d element array with random numbers.\n", size);
  start_time = getCurrentTime();
  for (i = 0; i < size; i++)
  {
    nums[i] = rand();
    // printf("%s\b",status[i%4]);
  }
  end_time = getCurrentTime();
  // printf("array population complete. (%gs)\n", end_time-start_time);

  // printf("starting ascending qsort of %d elements.\n", size);

  start_time = getCurrentTime();
  qsort(nums, size, sizeof(int), ascending);

  // printf("starting descending qsort of %d elements.\n", size);
  qsort(nums, size, sizeof(int), descending);
  end_time = getCurrentTime();
  fin_time = (double)(end_time - start_time);
   printf("%g", fin_time);

  return fin_time;
}
