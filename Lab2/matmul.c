#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#define NUM_THREADS 8

double **A,**B,**C;
int n;

void * mult(void * arg)
{
    int i,j,k;
    long t = (long)arg;

    for(i=t*n/NUM_THREADS;i<(t+1)*n/NUM_THREADS;i++)
	for (j=0;j<n;j++)
	    for (k=0;k<n;k++)
		C[i][j]+=A[i][k]*B[k][j];

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i,j,k,time,el;
    pthread_t threads[NUM_THREADS];
    double t;

    n = atoi(argv[1]);
  
    //Allocate and fill matrices
    A = (double **)malloc(n*sizeof(double *));
    B = (double **)malloc(n*sizeof(double *));
    C = (double **)malloc(n*sizeof(double *));

    for(i=0;i<n;i++){
	A[i] = (double *)malloc(n*sizeof(double));
	B[i] = (double *)malloc(n*sizeof(double));
	C[i] = (double *)malloc(n*sizeof(double));
    }

    for (i = 0; i<n; i++)
	for(j=0;j<n;j++){
	    A[i][j] = rand() % 5 + 1;
	    B[i][j] = rand() % 5 + 1;
	    C[i][j] = 0.0;
	}
    
    time=timer();
		  
    // Multiply C=A*B		  
    for(i=0;i<NUM_THREADS;i++)
    {
	el = i;
	pthread_create(&threads[i],NULL,mult,(void * )el);
    }
    for(i=0; i<NUM_THREADS; i++) 
	pthread_join(threads[i], NULL);
		  
    time=timer()-time;
    printf("Elapsed time: %f \n",time/1000000.0);
   
    return 0;
   
}
