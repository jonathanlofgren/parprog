/**********************************************************************
 * This program calculates pi using C
 *
 **********************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5
#define INTERVALS 100000000

void *work(void *arg)
{
  int myid = (int)arg;
  
  const int slice = INTERVALS / NUM_THREADS;
  int i;
  double sum, dx, x;
  dx  = 1.0/INTERVALS;
  sum = 0.0;

  for (i = (myid) * slice + 1; i <= (myid + 1) * slice; i++) { 
    x = dx*(i - 0.5);
    sum += dx*4.0/(1.0 + x*x);
  }
  printf("partial pi = %f\n", sum);
  pthread_exit((void*)&sum);
}

int main(int argc, char *argv[]) {

  pthread_t threads[NUM_THREADS];
  int t, tid[NUM_THREADS];
  double sum = 0;
  void *temp;

  for(t=0;t<NUM_THREADS;t++)
    {
      tid[t]=t;
      pthread_create(&threads[t], NULL, work, (void *)tid[t]);
    }
  for(t=0; t<NUM_THREADS; t++) 
    {
      pthread_join(threads[t], &temp);
      sum += *((double*)temp);
    }
  
  printf("PI is approx. %.16f\n",  sum);
  pthread_exit(NULL);
  return 0;
}
