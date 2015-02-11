#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5

void *HelloWorld(void *arg)
{
    int myid = pthread_self();
    printf("Hello World!\nMy id is: %d\n", myid);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int t;
    for(t=0;t<NUM_THREADS;t++)
	pthread_create(&threads[t], NULL, HelloWorld, NULL);
    pthread_exit(NULL);
}
