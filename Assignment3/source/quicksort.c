#include <stdio.h>
#include <stdlib.h>
#include "bucketsort.h"

#define SWAP(X, Y, TEMP) {TEMP = X; X = Y; Y = TEMP;}

void swap(double *a, double *b, double *temp);
void print(double *a, int n);
void quicksort(double *a, int p, int r);

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

void insertion_sort(double *a, int l, int r)
{
    int i = l+1, j;
    double temp;

    while (i <= r)
    {
	j = i;

	while (j > 0 && a[j] < a[j-1]) 
	{
	    SWAP(a[j-1], a[j], temp)
	    j--;
	}
	
	i++;
    }
}

void quicksort_cutoff(double *a, int p, int r)
{
    if (r-p < 15)
    {
	insertion_sort(a, p, r);
    }
    else
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

void swap_bucket(bucket *a, bucket *b, bucket *temp)
{
    *temp = *b;
    *b = *a;
    *a = *temp;
}

/* Sorts an array of buckets according to their count
 */
void quicksort_bucket(bucket *blist, int p, int r)
{
    if (p < r)
    {
	bucket pivot = blist[r];
	int i = p - 1;
	int j = p;
	bucket temp;

	while (j < r)
	{
	    if (blist[j].count <= pivot.count)
	    {
		i++;
		swap_bucket(&blist[i], &blist[j], &temp);
	    }
	
	    j++;
	}

	i++;
	swap_bucket(&blist[r], &blist[i], &temp);

	quicksort_bucket(blist, p, i-1);
	quicksort_bucket(blist, i+1, r);
    }
}

