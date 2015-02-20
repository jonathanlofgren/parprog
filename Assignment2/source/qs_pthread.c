#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
//#define PRINT

//#define MAX_THREADS 8
//#define MAX_LEVEL 5
int MAX_LEVEL;

void swap(double *a, double *b, double *temp);
void print(double *a, int n);
void *quicksort(void *arg);

volatile int running_threads = 0;
pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    int start;
    int end;
    double *a;
    int level;
} sort_arg;

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

void qs_serial(double *a, int p, int r)
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

	qs_serial(a, p, i-1);
	qs_serial(a, i+1, r);
    }
}

void *quicksort(void *sarg)
{
    sort_arg *arg = (sort_arg *)sarg;
    double *a = arg->a;
    int l = arg->start;
    int r = arg->end;
    int level = arg->level;

    if (l < r)
    {
	double pivot = a[r];
	int i = l - 1;
	int j = l;
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


	//if (running_threads < MAX_THREADS && (r-l) > 10)
	if (level < MAX_LEVEL)
	{
	    sort_arg *l_arg = (sort_arg *)malloc(sizeof(sort_arg));
	    l_arg->a = a;
	    l_arg->start = l;
	    l_arg->end = i-1;
	    l_arg->level = level+1;

	    sort_arg *r_arg = (sort_arg *)malloc(sizeof(sort_arg));
	    r_arg->a = a;
	    r_arg->start = i+1;
	    r_arg->end = r;
	    r_arg->level = level+1;

	    pthread_t newthread;
	    pthread_mutex_lock(&running_mutex);	    
	    running_threads++;
	    //printf("++:%d, level: %d\n", running_threads, level);
	    pthread_mutex_unlock(&running_mutex);
	    //printf("New thread created\n");
	    pthread_create(&newthread, NULL, quicksort, (void *)l_arg);
	    quicksort((void *)r_arg);
	}
	else
	{
	    //printf("level: %d\n", level);
	    pthread_mutex_unlock(&running_mutex);
	    qs_serial(a, l, i-1);
	    qs_serial(a, i+1, r);
	}

    }
    
    free(arg);

    pthread_mutex_lock(&running_mutex);
    running_threads--;
    //printf("--:%d\n", running_threads);
    pthread_mutex_unlock(&running_mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
 
    
    int n = atoi(argv[1]);
    int lvl = atoi(argv[2]);
    printf("Ordning och reda! lvl: %d\n", lvl);
    MAX_LEVEL = lvl;

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

    sort_arg *arg = (sort_arg *)malloc(sizeof(sort_arg));
    arg->a = a;
    arg->start = 0;
    arg->end = n-1;
    arg->level = 0;
    
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
   
    pthread_exit(NULL);
}
