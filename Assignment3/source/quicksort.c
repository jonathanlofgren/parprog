#include <stdio.h>
#include <stdlib.h>

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
