/**********************************************************************
 * Enumeration sort
 *
 **********************************************************************/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int timer(void);

int main(int argc, char *argv[]) {

    int len=50000,seed,i,j;
    double *indata, *outdata,wtime;
  
    indata=(double *)malloc(len*sizeof(double));
    outdata=(double *)malloc(len*sizeof(double));

    omp_set_nested(1);
    printf("Nested: %d\n", omp_get_nested());

    // Generate random numbers
    for (i=0;i<len;i++){
	indata[i]=drand48();
	outdata[i]=-1.0;
    }

    /* Enumeration sort */

    wtime=omp_get_wtime();

    int rank = 0;

#pragma omp parallel for firstprivate(rank)
    for (j=0;j<len;j++)
    {
#pragma omp parallel for reduction(+:rank)
	for (i=0;i<len;i++)
	{
	    //printf("Id: %d, %d, %d\n", omp_get_thread_num(),i,j);
	    
	    if (indata[i]<indata[j]) rank++;
	}
    	outdata[rank]=indata[j];
	rank=0;
    }

	
    wtime=omp_get_wtime()-wtime;
    printf("Time: %f sec \n",wtime);

    /* Check results, -1 implies data same as the previous element */
    for (i=0; i<len-1; i++)
	if (outdata[i]>outdata[i+1] && outdata[i+1]>-1)
	    printf("ERROR: %f,%f\n", outdata[i],outdata[i+1]);

    return 0;
}
