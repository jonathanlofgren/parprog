#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
//#define PRINT

void swap(double *a, double *b, double *temp);
void print(double *a, int n);
int partition(double *a, int p, int r);
void quicksort(double *a, int p, int r);

pthread_barrier_t   barrier; 

typedef struct 
{
  int tid;
  int p;
  int n;
  double *a;
} pear_arg;

int timer(void)
{
  struct timeval tv;
  gettimeofday(&tv, (struct timezone*)0);
  return (tv.tv_sec*1000000+tv.tv_usec);
}

void print(double *a, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
	printf("%0.2f ", a[i]);
    }

    printf("\n");
}

void swap(double *a, double *b, double *temp)
{
    *temp = *b;
    *b = *a;
    *a = *temp;
}

void merge(double * a, double * temp, int n, int l1, int l2)
{
    int i1 = 0, i2 = 0, i;
  
  for (i = 0; i < n; i++)
    {

      if (i1 >= n/2)
	{
	  temp[i] = a[l2+i2];
	  i2++;
	}
      else if (i2 >= n/2)
	{
	  temp[i] = a[l1+i1];
	  i1++;
	}
      else if (a[l1+i1] > a[l2+i2])
	{
	  temp[i] = a[l2+i2];
	  i2++;
	}
      else
	{
	  temp[i] = a[l1+i1];
	  i1++;
	}
    }

}

void * pearsort(void * arg)
{
  pear_arg * parg = (pear_arg *)arg;
  int tid = parg->tid;
  double * a = parg->a;
  int n = parg->n;
  int p = parg->p;
  int oddid = tid % 2;
  int block = n/p;
  double * temp = (double *)malloc(2*block*sizeof(double));
  int l = n/p*tid;
  int r = l + block - 1;

  int didsomething;

  // sort this threads block
  quicksort(a,l,r);

  pthread_barrier_wait(&barrier);
  
  int i;
  for (i = 0; i < p; i++)
    {
      int evenodd = i % 2;
      didsomething = 0;

      // merge odd-even blocks
      if (!evenodd && !oddid && tid != p-1)
	{
	  merge(a, temp, 2*block, l, l+block); //do something
	  didsomething = 1;
	}
      // merge even-odd blocks
      else if (evenodd && oddid && tid != p-1)
	{
	  merge(a, temp, 2*block, l, l+block);	 
 	  didsomething = 1;
	}

      pthread_barrier_wait(&barrier);

      // copy the merged blocks back to a
      if (didsomething)
	memcpy(&a[l],temp,2*block*sizeof(double));	
	
      pthread_barrier_wait(&barrier);

      if (tid == -1)
	{
	  printf("Step %d:  ", i);
	  print(a, n);
	}

    }
  
  free(temp);
  pthread_exit(NULL);
}

void quicksort(double *a, int p, int r)
{
    if (p < r)
    {
	double pivot = a[r];
	int i = p - 1;
	int j = p;
	double temp;

	while (j < r)
	{
	    if (a[j] <= pivot)
	    {
		i++;
		swap(&a[i], &a[j], &temp);
	    }
	
	    j++;
	}

	i++;
	swap(&a[r], &a[i], &temp);

	quicksort(a, p, i-1);
	quicksort(a, i+1, r);
    }
}

int main(int argc, char *argv[]) 
{
 
    int n = atoi(argv[1]);
    int p = atoi(argv[2]);
    printf("Ordning och reda! p: %d \n", p);
    pthread_barrier_init(&barrier, NULL, p);

    pthread_t threads[p];
    pear_arg pargarr[p];

    double *a = (double *)malloc(n*sizeof(double));
    
    srand(time(NULL));

    int i;
    for (i = 0; i < n; i++)
    {
	a[i] = drand48(); 
    }

#ifdef PRINT
    printf("Unsorted: ");
    print(a, n);
#endif

    int t1, t2;

    t1 = timer();   
    
    for (i = 0; i < p; i++)
      {
	pargarr[i].tid = i;
	pargarr[i].p = p;
	pargarr[i].n = n;
	pargarr[i].a = a;

	pthread_create(&threads[i], NULL, pearsort, (void *)&pargarr[i]);
      }

    for(i=0; i < p; i++) 
      {
	pthread_join(threads[i], NULL);
      }

    t2 = timer();

#ifdef PRINT
    printf("Sorted: ");
    print(a, n);
#endif

    printf("Elapsed time: %f ms \n",(t2-t1)/1000.0);

    for (i = 0; i < n-1; ++i)
    {
	if (a[i] > a[i+1])
	{
	    printf("Not sorted!\n\n");
	    break;
	}
    }
    
    if (i == n-1)
      printf("Pengar på fredag!\n\n");

    free(a);
}
