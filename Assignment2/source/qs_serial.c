#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define PRINT

void swap(double *a, double *b, double *temp);
void print(double *a, int n);
int partition(double *a, int p, int r);
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

int main(int argc, char *argv[]) 
{
    printf("Ordning och reda!\n");
    
    int n = atoi(argv[1]);
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

    clock_t t1, t2;

    t1 = clock();   
    quicksort(a, 0, n-1);
    t2 = clock();

#ifdef PRINT
    printf("Sorted: ");
    print(a, n);
#endif

    printf("Processor time: %0.3f ms\n", 
	   (double)(1000.0*(t2-t1))/CLOCKS_PER_SEC);
    
}
