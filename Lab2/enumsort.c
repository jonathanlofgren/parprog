/**********************************************************************
 * Enumeration sort
 *
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS	8
#define len 100000

double indata[len], outdata[len];

void *findrank(void *arg)
{
  int rank,i,k;
  long j=(long)arg;
  printf("%d\n",j*len/NUM_THREADS);	
  rank=0;
  for (k = j*len/NUM_THREADS; k <(j+1)*len/NUM_THREADS;k++)
    {
      rank = 0;
      for (i=0;i<len;i++)
	if (indata[i]<indata[k]) rank++;

      outdata[rank]=indata[k];
    }
  pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
	
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  int seed,i,j,rank,nthreads,ttime,t;
  long el;
  void *status;
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	

  // Generate random numbers
  for (i=0;i<len;i++){
    indata[i]=drand48();
    outdata[i]=-1.0;
  }


  // Enumeration sort
  ttime=timer();
  for(t=0; t<NUM_THREADS; t++) {
    el=t;
    pthread_create(&threads[t], &attr, findrank, (void *)el); 
  }
		
  for(t=0; t<NUM_THREADS; t++) 
    pthread_join(threads[t], &status);

  ttime=timer()-ttime;
  printf("Time: %f %d\n",ttime/1000000.0,NUM_THREADS);

  // Check results, -1 implies data same as the previous element
  for (i=0; i<len-1; i++)
    if (outdata[i]>outdata[i+1] && outdata[i+1]>-1)
      printf("ERROR: %f,%f\n", outdata[i],outdata[i+1]);

  return 0;
}
