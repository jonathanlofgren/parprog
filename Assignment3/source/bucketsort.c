/**********************************************************************
 * Bucketsort using OpenMP/C
 *
 **********************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

//#define PRINT

typedef struct Bucket
{
    int count;
    int size;
    double *b;
} bucket;

int main(int argc, char *argv[]) 
{
    printf("Threads = %d\n", omp_get_max_threads());

    int bucket_count = 1000;
    int n = 10000000;

    int bucket_size = 2*n/bucket_count;

    double *a = (double *)malloc(n*sizeof(double));

    int i;
    srand48(time(NULL));
    for (i = 0; i < n; i++)
    {
	a[i] = drand48();
    }

    // start timing here!
    double timer1;
    timer1=omp_get_wtime();

    // allocate the buckets
    bucket *blist = (bucket *)malloc(bucket_count*sizeof(bucket));
    for (i = 0; i < bucket_count; i++)
    {
	blist[i].b = (double *)malloc(bucket_size*sizeof(double));
	blist[i].count = 0;
	blist[i].size = bucket_size;
    }

    int pos, count;
    // assign the elements to the buckets
    for (i = 0; i < n; i++)
    {
	pos = (int)floor(bucket_count*a[i]);
	count = blist[pos].count;
	
	// reallocate if bucket full
	if (count == blist[pos].size)
	{
	    blist[pos].size *= 2;
	    double *bnew = (double *)malloc(blist[pos].size*sizeof(double));
	    memcpy(bnew, blist[pos].b, count*sizeof(double));
	    free(blist[pos].b);
	    blist[pos].b = bnew;

	}

	blist[pos].b[count] = a[i];
	blist[pos].count++;
    }

#ifdef PRINT
    print(a, n);
    for (i = 0; i < bucket_count; i++)
    {
	printf("size: %d, count: %d ## ", blist[i].size, blist[i].count);
	print(blist[i].b, blist[i].count);
    }
#endif
    
    // quicksort each bucket
    for (i = 0; i < bucket_count; i++)
    {
	quicksort(blist[i].b, 0, blist[i].count - 1);
    }

#ifdef PRINT
    printf("Sorted buckets: \n");
    for (i = 0; i < bucket_count; i++)
    {
	printf("size: %d, count: %d ## ", blist[i].size, blist[i].count);
	print(blist[i].b, blist[i].count);
    }
#endif

    int apos = 0;
    for (i = 0; i < bucket_count; i++)
    {
	memcpy(&a[apos], blist[i].b, blist[i].count*sizeof(double));
	apos += blist[i].count;
    }

    timer1=omp_get_wtime()-timer1;
    printf("Time: %f sec\n",timer1);

#ifdef PRINT
    printf("Sorted a: ");
    print(a, n);
#endif

    // check that the array is sorted
    for (i = 0; i < n-1; i++)
    {
	if (a[i] > a[i+1])
	{
	    printf("Error at position %d.\n", i);
	    break;
	}
    }

    if (i == n-1)
	printf("Pengar på fredag!\n");
}
