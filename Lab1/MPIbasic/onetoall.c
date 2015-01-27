/**********************************************************************
 * Point-to-point communication using MPI
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int rank, size,i;
  double a;
  MPI_Status status;

  MPI_Init(&argc, &argv);               /* Initialize MPI               */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */
  
  /* Pass on sequence */

  /**
  if(rank == 0){
    a=999.999;
    MPI_Send(&a, 1, MPI_DOUBLE, rank + 1, 111, MPI_COMM_WORLD);
  }
  else {
    MPI_Recv(&a, 1, MPI_DOUBLE, rank -1, 111, MPI_COMM_WORLD, &status);
    printf("Processor %d got %f\n", rank,a);

    if (rank != size-1){
      MPI_Send(&a, 1, MPI_DOUBLE, rank + 1, 111, MPI_COMM_WORLD);
    }
  }
  /**/

  /* Fan-out sequence */

  /**
  if (rank==0){
    a = 999.999;
  }
  int count = 1;
  
  while (count < size){
    
    if (rank < count && rank + count < size){
      MPI_Send(&a, 1, MPI_DOUBLE, rank + count, 111, MPI_COMM_WORLD);
    }
    
    if (rank >= count && rank < count * 2){
      MPI_Recv(&a, 1, MPI_DOUBLE, rank - count, 111, MPI_COMM_WORLD, &status);
      printf("Processor %d got %f\n", rank, a);
    }
    
    count *= 2;
  }
  /**/

  
  /* Processor 0 to all */

  /**
  if (rank == 0) {
    a=999.999;
    for (i=0;i<size;i++)
      MPI_Send(&a, 1, MPI_DOUBLE, i, 111, MPI_COMM_WORLD);
  } else {
    MPI_Recv(&a, 1, MPI_DOUBLE, 0, 111, MPI_COMM_WORLD, &status);
    printf("Processor %d got %f\n", rank,a);
  }
  /**/

  /* Broadcast */

  if (rank == 0) {
    a = 1337.1337;
  }

  MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  printf("Processor %d got %f\n", rank,a);

  MPI_Finalize(); 

  return 0;
}
