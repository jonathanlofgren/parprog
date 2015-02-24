/**********************************************************************
 * Enumeration sort
 *
 **********************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int timer(void);

int main(int argc, char *argv[]) {

    int len=50000,seed,i,j,rank;
    double *indata, *outdata,wtime;
  
    indata=(double *)malloc(len*sizeof(double));
    outdata=(double *)malloc(len*sizeof(double));


    // Generate random numbers
    for (i=0;i<len;i++){
	indata[i]=drand48();
	outdata[i]=-1.0;
    }

    /* Enumeration sort */

    wtime=omp_get_wtime();


#pragma omp parallel for private(rank)
    for (j=0;j<len;j++)
    {
	rank=0;
	//printf("number: %d reporting in\n", omp_get_thread_num());
#pragma omp parallel for reduction(+:rank)
	for (i=0;i<len;i++)
	{
	    if (indata[i]<indata[j]) rank++;
	    printf("number: %d reporting in\n", omp_get_thread_num());
	}
	outdata[rank]=indata[j];
    }
	
    wtime=omp_get_wtime()-wtime;
    printf("Time: %f sec \n",wtime);

    /* Check results, -1 implies data same as the previous element */
    for (i=0; i<len-1; i++)
	if (outdata[i]>outdata[i+1] && outdata[i+1]>-1)
	    printf("ERROR: %f,%f\n", outdata[i],outdata[i+1]);

    return 0;
}
