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

#define PI 3.14159265358979323846

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
    


// Using tasks
/**/
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

// Dynamic scehduling
/**
#pragma parallel for schedule(dynamic,1)
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
	printf("Pengar p� fredag!\n");

    // Clean up!
    for (i = 0; i < bucket_count; i++)
	free(blist[i].b);

    free(a);
    free(blist);
}
