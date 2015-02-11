#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define PRINT

void swap(int *a, int *b);
void print(int *a, int n);
int partition(int *a, int p, int r);
void quicksort(int *a, int p, int r);


void print(int *a, int n)
{
    for (int i = 0; i < n; i++)
    {
	printf("%d ", a[i]);
    }

    printf("\n");
}

void swap(int *a, int *b)
{
    int temp = *b;
    *b = *a;
    *a = temp;
}

int partition(int *a, int p, int r)
{
    // pivot value
    int pivot = a[r];
    //printf("Pivot: %d \n", pivot);

    int i = p - 1;

    for (int j = p; j < r; j++)
    {
	if (a[j] <= pivot)
	{
	    //printf("test %d\n", j);
	    i++;
	    swap(&a[i], &a[j]);
	}
    }

    swap(&a[r], &a[i+1]);
    return i+1;
}

void quicksort(int *a, int p, int r)
{
    if (p < r)
    {
	int q = partition(a, p, r);
	quicksort(a, p, q-1);
	quicksort(a, q+1, r);
    }
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
