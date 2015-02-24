/**********************************************************************
 * This program calculates pi using OpenMP/C
 *
 **********************************************************************/

#include <omp.h>
#include <stdio.h>

int main(int argc, char *argv[]) 
{
    printf("maxnumthreads = %d\n", omp_get_max_threads());
    int rank, size, i;
    const int intervals = 100000000L ;
    double sum, dx, x;

    dx  = 1.0/intervals;
    sum = 0.0;

    double timer1;
    timer1=omp_get_wtime();

#pragma omp parallel for reduction(+:sum) //private(x)
    for (i = 1; i <= intervals; i++) { 
	//x = dx*(i - 0.5);
	sum += dx*4.0/(1.0 + dx*(i - 0.5)*dx*(i - 0.5));
	//printf("I am number %d!\n", omp_get_thread_num());
    }
    
    timer1=omp_get_wtime()-timer1;
    printf("Time: %f sec\n",timer1);

    printf("PI is approx. %.16f\n",  sum);

    return 0;
}
