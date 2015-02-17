#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
//#define PRINT

#define MAX_THREADS 32

void swap(int *a, int *b, int *temp);
void print(int *a, int n);
int partition(int *a, int p, int r);
void *quicksort(void *arg);

volatile int running_threads = 0;
volatile int MAX_REACHED = 0;
pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    int start;
    int end;
    int *a;
} sort_arg;

int timer(void)
{
  struct timeval tv;
  gettimeofday(&tv, (struct timezone*)0);
  return (tv.tv_sec*1000000+tv.tv_usec);
}

void print(int *a, int n)
{
    for (int i = 0; i < n; i++)
    {
	printf("%d ", a[i]);
    }

    printf("\n");
}

void swap(int *a, int *b, int *temp)
{
    *temp = *b;
    *b = *a;
    *a = *temp;
}

void qs_serial(int *a, int p, int r)
{
    if (p < r)
    {
	int pivot = a[r];
	int i = p - 1;
	int j = p;
	int temp;

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

	qs_serial(a, p, i-1);
	qs_serial(a, i+1, r);
    }
}

void *quicksort(void *sarg)
{
    sort_arg *arg = (sort_arg *)sarg;
    int *a = arg->a;
    int l = arg->start;
    int r = arg->end;

    if (l < r)
    {
	int pivot = a[r];
	int i = l - 1;
	int j = l;
	int temp;

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

	sort_arg *l_arg = (sort_arg *)malloc(sizeof(sort_arg));
	l_arg->a = a;
	l_arg->start = l;
	l_arg->end = i-1;

	sort_arg *r_arg = (sort_arg *)malloc(sizeof(sort_arg));
	r_arg->a = a;
	r_arg->start = i+1;
	r_arg->end = r;

	pthread_t threads[2];
	
	pthread_mutex_lock(&running_mutex);
	if (running_threads < MAX_THREADS && !MAX_REACHED && (r-l) > 10)
	{
	 
	    running_threads++;
	    printf("++:%d\n", running_threads);
	    pthread_mutex_unlock(&running_mutex);
	    pthread_create(&threads[0], NULL, quicksort, (void *)l_arg);

	    quicksort((void *)r_arg);
	}
	else
	{
	    MAX_REACHED = 1;
	    pthread_mutex_unlock(&running_mutex);
	    qs_serial(a, l, i-1);
	    qs_serial(a, i+1, r);
	}

    }
    
    free(arg);

    pthread_mutex_lock(&running_mutex);
    running_threads--;
    printf("--:%d\n", running_threads);
    pthread_mutex_unlock(&running_mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
    printf("Ordning och reda!\n");
    
    int n = atoi(argv[1]);
    int *a = (int *)malloc(n*sizeof(int));
    
    srand(time(NULL));

    for (int i = 0; i < n; i++)
    {
	a[i] = 1+rand()%100; 
    }

#ifdef PRINT
    printf("Unsorted: ");
    print(a, n);
#endif

    int t1, t2;

    t1 = timer();   

    sort_arg *arg = (sort_arg *)malloc(sizeof(sort_arg));
    arg->a = a;
    arg->start = 0;
    arg->end = n-1;
    
    pthread_t t;
    running_threads++;
    pthread_create(&t, NULL, quicksort, (void *)arg);
 
#ifdef PRINT
    printf("Sorted: ");
    print(a, n);
#endif
    
    while (running_threads > 0)
    {
	usleep(1);
    }

    t2 = timer();
    printf("Elapsed time: %f s \n",(t2-t1)/1000000.0);
    
    for (int i = 0; i < n-1; ++i)
    {
	if (a[i] > a[i+1])
	{
	    printf("Not sorted!\n");
	    break;
	}
    }
   
    pthread_exit(NULL);
}
