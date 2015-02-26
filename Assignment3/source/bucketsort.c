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
#include "bucketsort.h"

#define PI 3.14159265358979323846

//#define PRINT


void bucket_balance(bucket *blist, int bucket_count, bucketbucket *bb, int num_threads)
{
    bucket *sorted_buckets = (bucket *)malloc(bucket_count*sizeof(bucket));
    memcpy(sorted_buckets, blist, bucket_count*sizeof(bucket)); 
    
    quicksort_bucket(sorted_buckets, 0, bucket_count-1);
    
    int i, j, least;
    double least_load, load;

    for (i = bucket_count - 1; i >= 0; i--)
    {
	// Get work load for this bucket
	if (sorted_buckets[i].count != 0)
	{
	    load = sorted_buckets[i].count*log(sorted_buckets[i].count);
	}	

	// find the bucketbucket with the least load
	least = 0;
	least_load = bb[0].load;
	for (j = 1; j < num_threads; j++)
	{
	    if (bb[j].load < least_load)
	    {
		least = j;
		least_load = bb[j].load;
	    }
	}

	// insert the bucket
	bb[least].buckets[bb[least].bucket_count] = sorted_buckets[i];
	bb[least].load += load;
	bb[least].bucket_count++;


	//printf("Count: %d\n", sorted_buckets[i].count);
	
    }
    
    printf("\nLoad balancing:\n");
    for (i = 0; i < num_threads; i++)
    {
	printf("Load: %0.2f\n", bb[i].load);
    }
    printf("\n");

    free(sorted_buckets);
}

bucketbucket *create_bucketbucket(int num, int bucket_count)
{
    bucketbucket *bb = (bucketbucket *)malloc(num*sizeof(bucketbucket));
    
    int i;
    for (i = 0; i < num; i++)
    {
	bb[i].load = 0;
	bb[i].bucket_count = 0;
	bb[i].buckets = (bucket *)malloc(bucket_count*sizeof(bucket));
    }

    return bb;
}

void free_bucketbucket(bucketbucket *bb, int num)
{
    int i;
    for (i = 0; i < num; i++)
    {
	free(bb[i].buckets);
    }

    free(bb);
}

int main(int argc, char *argv[]) 
{
    omp_set_num_threads(4);
    int num_threads = omp_get_max_threads();
    printf("Threads = %d\n", num_threads);

  

    if (argc != 3)
    {
	printf("Usage: ./bucketsort buckets arraylength\n");
	return 1;
    }

    int bucket_count = atoi(argv[1]);
    int n = atoi(argv[2]);
    int bucket_size = 2*n/bucket_count;

    double *a = (double *)malloc(n*sizeof(double));

    int i;
    srand48(time(NULL));
    /*
    for (i = 0; i < n; i++)
    {
	a[i] = drand48();
    }
    */
    
    // normal distribution
    double u1, u2;
    for (i = 0; i < n; i+=2)
    {
	u1 = drand48();
	u2 = drand48();
	a[i] = sqrt(-2.0*log(u1))*cos(2*PI*u2);
	a[i+1] = sqrt(-2.0*log(u1))*sin(2*PI*u2);
    }

    // start timing here!
    double timer1, start;
    timer1=omp_get_wtime();
    start = timer1;

    // allocate the buckets
    bucket *blist = (bucket *)malloc(bucket_count*sizeof(bucket));

    for (i = 0; i < bucket_count; i++)
    {
	blist[i].b = (double *)malloc(bucket_size*sizeof(double));
	blist[i].count = 0;
	blist[i].size = bucket_size;
    }

    printf("Allocate bucket time: %0.4f sec\n", omp_get_wtime()-timer1);
    timer1=omp_get_wtime();

    int pos, count;
    double offset = 3, temp;
    // assign the elements to the buckets
    for (i = 0; i < n; i++)
    {
	temp = a[i]+offset;
	if (temp < 0)
	    pos = 0;
	else if (temp > offset*2)
	    pos = bucket_count - 1;
	else
	    pos = (int)floor(temp/(offset*2)*(bucket_count-2)) + 1;
	
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

    printf("Assigning to bucket time: %0.4f sec\n", omp_get_wtime()-timer1);
    timer1=omp_get_wtime();

#ifdef PRINT
    print(a, n);
    for (i = 0; i < bucket_count; i++)
    {
	printf("size: %d, count: %d ## ", blist[i].size, blist[i].count);
	print(blist[i].b, blist[i].count);
    }
#endif
    
/* Manual load balancing */
    
    bucketbucket *bb = create_bucketbucket(num_threads, bucket_count);
    bucket_balance(blist, bucket_count, bb, num_threads);
    
    int tid;
#pragma omp parallel private(i, tid) num_threads(num_threads)
    {
	int startime = omp_get_wtime();
	tid = omp_get_thread_num();

	for (i = 0; i < bb[tid].bucket_count; i++)
	{
	    quicksort(bb[tid].buckets[i].b, 0,bb[tid].buckets[i].count - 1);
	}

	printf("Tid: %0.5f\n", omp_get_wtime()-startime);
    }

      free_bucketbucket(bb, num_threads);
/**/

/* Serial *
    for (i = 0; i < bucket_count; i++)
    {
	quicksort(blist[i].b, 0, blist[i].count - 1);    
    }
/**/

/* Static parallel for *
#pragma omp parallel for
    for (i = 0; i < bucket_count; i++)
    {
	quicksort(blist[i].b, 0, blist[i].count - 1);

    }
/**/

  
/* One thread for each bucket *
    int tid;
#pragma omp parallel private(tid) num_threads(bucket_count)
    {
	tid = omp_get_thread_num();
	quicksort(blist[tid].b, 0, blist[tid].count - 1);    
    }
/**/


/* Using tasks *
#pragma omp parallel
    {
#pragma omp single
	{
	    for (i = 0; i < bucket_count; i++)
	    {
#pragma omp task firstprivate(i)
		{
		    quicksort(blist[i].b, 0, blist[i].count - 1);
		}	    
	    }
	}
    }
/**/


/* Dynamic scheduling *
#pragma omp parallel for schedule(dynamic,1)
    for (i = 0; i < bucket_count; i++)
    {
	quicksort(blist[i].b, 0, blist[i].count - 1);

    }
/**/

    printf("Quicksort buckets time: %0.4f sec\n", omp_get_wtime()-timer1);
    timer1=omp_get_wtime();

#ifdef PRINT
    printf("Sorted buckets: \n");
    for (i = 0; i < bucket_count; i++)
    {
	printf("size: %d, count: %d ## ", blist[i].size, blist[i].count);
	print(blist[i].b, blist[i].count);
    }
#endif

    // copy the sorted buckets back to a
    int apos = 0;
    for (i = 0; i < bucket_count; i++)
    {
	memcpy(&a[apos], blist[i].b, blist[i].count*sizeof(double));
	apos += blist[i].count;
    }

    printf("Copy back time: %0.4f sec\n", omp_get_wtime()-timer1);
    timer1=omp_get_wtime();

    timer1=timer1-start;
    printf("Toal time: %f sec\n",timer1);

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

    // Clean up!
    for (i = 0; i < bucket_count; i++)
	free(blist[i].b);

    free(a);
  
    free(blist);

}
