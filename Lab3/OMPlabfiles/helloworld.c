/**********************************************************************
 * A simple "hello world" program for OpenMP/C
 *
 *
 **********************************************************************/

#include <omp.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    int total = omp_get_max_threads();
    printf("Starting\n");
    printf("Max num threads: %d\n", total);

#pragma omp parallel 
  {
      int id = omp_get_thread_num();
      printf("Hello World! This is thread %d writing! :D\n", id); 
  }

  printf("Stopping\n"); 

  return 0;
}
