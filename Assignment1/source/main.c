#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MASTER 0

int main(int argc, char *argv[]) {

  int n, grid_rank, nproc;
  int ndim=2, dims[2]={0,0}, reorder=1, periods[2] = {1,1};
  MPI_Comm proc_grid;
  double *A, *B;
  
  // Initialize MPI  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
  // Create a virtual 2D-grid topology 
  MPI_Dims_create(nproc, ndim, dims);
  MPI_Cart_create(MPI_COMM_WORLD, ndim, dims, periods, reorder, &proc_grid);
  MPI_Comm_rank(proc_grid, &grid_rank);

  // Check for valid input
  if (argc != 2)
  {
      if (grid_rank == MASTER) printf("Usage: mpifox n \n\n"
	                              "    n: matrix size \n");
 
      MPI_Finalize();
      exit(0);
  }
  else
  {
      n = atoi(argv[1]);
      if (grid_rank == MASTER) printf("Matrix size: %dx%d \n", n, n);
  }
  
  
  // Master process creates matrix A and B
  if (grid_rank == MASTER)
  {
    n=atoi(argv[1]);
    A=(double *)calloc(n*n,sizeof(double));
    B=(double *)calloc(n*n,sizeof(double));

    time_t t;
    srand((unsigned) time(&t));
    
    for (int i=0; i<n*n; i++)
    {
      A[i]=((double)rand()/(double)RAND_MAX);
      B[i]=((double)rand()/(double)RAND_MAX);
    }
  }

  // CODE GOES HERE


  if (grid_rank == MASTER)
  {
    free(A);
    free(B);
  }

  MPI_Finalize();
  return 0;
}

  /**
  int myid,nproc,rank,i,j,subrank,data,mydata;
  MPI_Comm proc_grid, proc_row;
  int coords[2],pos[2],reorder=1,ndim=2,dims[2]={0,0},periods[2]={0,0};
  MPI_Status stat;

  // Initialize MPI  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  // Create a virtual 2D-grid topology 
  MPI_Dims_create(nproc,ndim,dims);
  MPI_Cart_create(MPI_COMM_WORLD,ndim,dims,periods,reorder,&proc_grid);
  MPI_Comm_rank(proc_grid,&myid);    // Note: use proc_grid

  // Create a communicator for each row 
  MPI_Cart_coords(proc_grid,myid,ndim,coords); 
  MPI_Comm_split(proc_grid,coords[0],coords[1],&proc_row);
  MPI_Comm_rank(proc_row,&subrank); 

  // Broadcast within a row 
  if (subrank==0)
    mydata=coords[0];
  MPI_Bcast(&mydata,1,MPI_INT,0,proc_row);

  // Check the result of Broadcast
  for (i=0; i<dims[0]; i++){
    for (j=0; j<dims[1]; j++){
      pos[0]=i; pos[1]=j;
      MPI_Cart_rank(proc_grid,pos,&rank);
	if (rank==myid)
	  printf("Rank: %d, Coords: %d %d, Data: %d\n", myid,i,j,mydata);
    }
  }

  // Exit and clean up MPI variables
  MPI_Comm_free(&proc_row);
  MPI_Comm_free(&proc_grid);

  /**/
