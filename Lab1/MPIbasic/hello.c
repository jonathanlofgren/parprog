/**********************************************************************
 * A simple "hello world" program for MPI/C
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);               /* Initialize MPI               */

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n;
  MPI_Comm_rank(MPI_COMM_WORLD, &n);

  
  if (n == 0){
    printf("Running on %i processors!\n",size);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  printf("Hello World! This is processor %i/%i.\n", n, size);             /* Print a message              */

  MPI_Finalize();                       /* Shut down and clean up MPI   */

  return 0;
}
