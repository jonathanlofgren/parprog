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


/* Distributes the buckets in blist into groups in bb 
 * to balance the total work required to sort 
 * each group of buckets.
 */
void bucket_balance(bucket *blist, int bucket_count, 
		    bucketbucket *bb, int num_threads)
{
    bucket *sorted_buckets = (bucket *)malloc(bucket_count*sizeof(bucket));
    memcpy(sorted_buckets, blist, bucket_count*sizeof(bucket)); 
    
    quicksort_bucket(sorted_buckets, 0, bucket_count-1);
    
    int i, j, least;
    double least_load, load;

    for (i = bucket_count - 1; i >= 0; i--)
    {
	// estimate work with n*log n
	if (sorted_buckets[i].count != 0)
	{
	    load = sorted_buckets[i].count*log(sorted_buckets[i].count);
	    //load = sorted_buckets[i].count;
	    //load = sorted_buckets[i].count*sorted_buckets[i].count;
	    //load = sqrt(sorted_buckets[i].count);
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
    }
    
    //printf("\nLoad balancing:\n");
    for (i = 0; i < num_threads; i++)
    {
	//printf("Bucket %d: %0.2f\n", i, bb[i].load);
    }
    //printf("\n");

    free(sorted_buckets);
}


/* Allocate num bucketbuckets with each with
 * room for bucket_count buckets
 */
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


/* Frees the bucketbuckets and all buckets it contains 
 */
void free_bucketbucket(bucketbucket *bb, int num)
{
    int i;
    for (i = 0; i < num; i++)
    {
	free(bb[i].buckets);
    }

    free(bb);
}


/* Fills the array a with n normal distributed 
 * random numbers
 */
void fill_normal_dist(double *a, int n)
{
    srand48(time(NULL));
    double u1, u2;
    int i;

    for (i = 0; i < n; i+=2)
    {
	u1 = drand48();
	u2 = drand48();
	a[i] = sqrt(-2.0*log(u1))*cos(2*PI*u2);
	a[i+1] = sqrt(-2.0*log(u1))*sin(2*PI*u2);
    } 
}


/* Sort the buckets in buckets by manually balancing
 * the buckets into groups and sorting each group of buckets
 * on a separate thread. 
 */
void sort_manual_loadbalance(bucket *buckets, int bucket_count,
			     int num_threads)
{
    bucketbucket *bb = create_bucketbucket(num_threads, bucket_count);
    bucket_balance(buckets, bucket_count, bb, num_threads);
    
    printf("Timings:\n");
    double starttime2 = omp_get_wtime();

#pragma omp parallel num_threads(num_threads)
    {
	double startime = omp_get_wtime();
	int tid = omp_get_thread_num();
	int i;
	for (i = 0; i < bb[tid].bucket_count; i++)
	{
	    quicksort(bb[tid].buckets[i].b, 0,bb[tid].buckets[i].count - 1);
	}

	printf("Thread %d: %0.5f sec\n", tid, omp_get_wtime()-startime);
    }
    printf("## Sorting time: %0.5f\n", omp_get_wtime()-starttime2);
    //printf("\n");
    free_bucketbucket(bb, num_threads);
}


/* Sort the buckets by creating a task for each sort
 * and letting openmp handle the load balancing
 */
void sort_tasks(bucket *buckets, int bucket_count)
{
#pragma omp parallel
    {
#pragma omp single
	{
	    int i;
	    for (i = 0; i < bucket_count; i++)
	    {
#pragma omp task firstprivate(i)
		{
		    quicksort(buckets[i].b, 0, buckets[i].count - 1);
		}	    
	    }
	}
    }   
}


/* Sort the buckets serially 
 */
void sort_serial(bucket *buckets, int bucket_count)
{
    int i;
    for (i = 0; i < bucket_count; i++)
    {
	quicksort(buckets[i].b, 0, buckets[i].count - 1);    
    } 
}


/* Sort the buckets by creating a separate thread 
 * for each buckets and letting the run time system
 * handle the work load.
 */
void sort_run_time_system(bucket *buckets, int bucket_count)
{
    int tid;
#pragma omp parallel private(tid) num_threads(bucket_count)
    {
	tid = omp_get_thread_num();
	quicksort(buckets[tid].b, 0, buckets[tid].count - 1);    
    }   
}


/* Sort the buckets in parallel for loop and let
 * openmp do the load balancing with the schedule directive.
 */
void sort_omp_scheduling(bucket *buckets, int bucket_count)
{
    int i;
#pragma omp parallel for schedule(dynamic, 1)
    for (i = 0; i < bucket_count; i++)
    {
	quicksort(buckets[i].b, 0, buckets[i].count - 1);

    } 
}


int main(int argc, char *argv[]) 
{
    //omp_set_num_threads(4);
    int num_threads = omp_get_max_threads();
    printf("Threads = %d\n", num_threads);

    if (argc != 4)
    {
	printf("Usage: ./bucketsort buckets arraylength method\n");
	return 1;
    }

    int bucket_count = atoi(argv[1]);
    int n = atoi(argv[2]);
    int method = atoi(argv[3]);
    int bucket_size = 2*n/bucket_count;

    double *a = (double *)malloc(n*sizeof(double));
    fill_normal_dist(a, n);

    // start timing here!
    double timer1, start;
    timer1=omp_get_wtime();
    start = timer1;

    // Allocate the buckets
    bucket *blist = (bucket *)malloc(bucket_count*sizeof(bucket));
    int i;
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

    // Assign the elements to the buckets
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
    

    switch(method){
    case 1  :
	/* Dynamic scheduling */
	sort_omp_scheduling(blist, bucket_count);
	break;
    case 2  :
	/* Using tasks */
	sort_tasks(blist, bucket_count);
	break; /* optional */
    case 3  :
        /* Manual load balancing */
	sort_manual_loadbalance(blist, bucket_count, num_threads);
	break; /* optional */
    case 4  :
	/* One thread for each bucket */
	sort_run_time_system(blist, bucket_count);
	break; /* optional */
    }
    
    printf("## Method %d\n", method);
    printf("## Quicksort buckets time: %0.4f sec\n", 
	   omp_get_wtime()-timer1);
    timer1=omp_get_wtime();

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
    printf("Total time: %f sec\n",timer1);


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
	printf("Pengar på fredag!\n\n");

    // Clean up!
    for (i = 0; i < bucket_count; i++)
	free(blist[i].b);
    free(a);
    free(blist);


    /*
    n = 10000000;
    a = (double *)malloc(n*sizeof(double));
    for (i=0; i<n; i++)
    {
	a[i] = drand48();
    }

    printf("\nSort testing:\n\n");

    timer1=omp_get_wtime();
    quicksort_cutoff(a, 0, n-1);
    printf("Sort test time: %0.4f sec\n", omp_get_wtime()-timer1);

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
    */
	
}

