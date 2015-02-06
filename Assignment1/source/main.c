#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MASTER 0
#define bl2 (blocksize*blocksize)

void print_matrix(double *matrix, int n)
{
  for (int i=0; i<n; i++)
  {
      for (int j=0; j<n; j++)
      {
	  printf("%0.8f ", matrix[i*n+j]);
      }
      printf("\n");
  }
  printf("\n");  
}

int matrix_equal(double *a, double *b, int n)
{
    int e = 0;

    for (int i = 0; i < n*n; i++)  
    {	
	if ((a[i] - b[i])*(a[i] - b[i]) > 0.1)
	{
	    printf("%0.6f != %0.6f \n", a[i], b[i]);
	    e++;
	}      
    } 

   return e;
}

void add_multiply(double *c, double *a, double *b, int n)
{
    for (int i = 0; i < n; i++)  
    {	
	for (int j = 0; j < n; j++)
	{
	    double sum = 0;

	    for (int k = 0; k < n; k++) {
		sum += a[i*n+k] * b[k*n+j];
	    }

	    c[i*n+j] += sum;
	}
    }
}

int main(int argc, char *argv[]) {

  int n, grid_rank, nproc;
  int ndim=2, dims[2]={0,0}, reorder=1, periods[2] = {1,1};
  MPI_Comm proc_grid;
  double *A, *B, *C;
  
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
    
  // Master process creates matrix A, B and C
  if (grid_rank == MASTER)
  {
    n=atoi(argv[1]);
    A=(double *)calloc(n*n, sizeof(double));
    B=(double *)calloc(n*n, sizeof(double));
    C=(double *)calloc(n*n, sizeof(double));

    srand((unsigned) time(NULL));
    //srand(42);

    for (int i=0; i<n*n; i++)
    {
	A[i]=(double)rand()/(double)RAND_MAX;
	B[i]=(double)rand()/(double)RAND_MAX;
	
	A[i] = (double)((int)(10*A[i]))/10;
	B[i] = (double)((int)(10*B[i]))/10;
    }

    //print_matrix(A, n);
    //print_matrix(B, n);
  }
  
  int coords[2], col_rank, row_rank;
  MPI_Comm proc_row, proc_col;
  MPI_Status status;

  // Create communicators for row and column
  MPI_Cart_coords(proc_grid, grid_rank, ndim, coords); 
  MPI_Comm_split(proc_grid,coords[1],coords[0],&proc_col);
  MPI_Comm_split(proc_grid,coords[0],coords[1],&proc_row);
  MPI_Comm_rank(proc_col, &col_rank);
  MPI_Comm_rank(proc_row, &row_rank);

  // Allocate the blocks
  int blocksize = n / dims[0]; 
  double *a, *b, *c, *a_temp;
  a = (double *)calloc(bl2, sizeof(double));
  a_temp = (double *)calloc(bl2, sizeof(double));
  b = (double *)calloc(bl2, sizeof(double));
  c = (double *)calloc(bl2, sizeof(double));

  // Create datatype for the blocks
  MPI_Datatype block_type;
  MPI_Type_vector(blocksize,blocksize,n,MPI_DOUBLE,&block_type);
  MPI_Type_commit(&block_type);

  // Send one block of A and B to each processor
  if (grid_rank == MASTER)
  {
      for (int i=0; i<nproc; i++)
      {	  
	  int x = (i % dims[0]) * blocksize;
	  int y = i / dims[0] * blocksize;
	  
	  MPI_Send(&A[y*n + x], 1, block_type, i, 42, proc_grid);
	  MPI_Send(&B[y*n + x], 1, block_type, i, 42, proc_grid);
      }
  }
  MPI_Recv(a, bl2, MPI_DOUBLE, 0, 42, proc_grid, &status);
  MPI_Recv(b, bl2, MPI_DOUBLE, 0, 42, proc_grid, &status);

  
  //printf("grid_rank: %d, coords: %d %d\n",grid_rank,coords[0],coords[1]);
  //print_matrix(a,blocksize);


  int m, source, dest;
  MPI_Request request, request2;

  // Run fox's algorithm to calculate c in each processor
  for (int k = 0; k < dims[0]; k++)
  {
      m = (coords[0]+k) % dims[0];

      if (row_rank == m)
      {
	  memcpy(a_temp, a, bl2*sizeof(double));	
      }
      
      MPI_Bcast(a_temp, bl2, MPI_DOUBLE, m, proc_row);
      
      MPI_Cart_shift(proc_grid, 0, -1, &source, &dest);
      MPI_Isend(b, bl2, MPI_DOUBLE, dest, 43, proc_grid, &request);
      
      add_multiply(c,a_temp,b,blocksize);
      
      // something wrong here with the communication?!

      MPI_Irecv(b, bl2, MPI_DOUBLE, source, 43, proc_grid, &request2);
      MPI_Wait(&request2, MPI_STATUS_IGNORE);
      MPI_Barrier(proc_grid);      
  }

  // Send all blocks c to MASTER
  MPI_Isend(c, bl2, MPI_DOUBLE, 0, 44, proc_grid, &request);
  
  if (grid_rank == MASTER)
  {
      for (int i=0; i<nproc; i++)
      {	  
	  int x = (i % dims[0]) * blocksize;
	  int y = i / dims[0] * blocksize;
	  
	  MPI_Irecv(&C[y*n + x], 1, block_type, i, 44, proc_grid, &request2);
	  MPI_Wait(&request2, MPI_STATUS_IGNORE);
      }
  }
  
  
  // Print results
  if (grid_rank == MASTER)
  {
      //printf("Result from fox:\n");
      //print_matrix(C, n);
      
      //printf("Result from BRUTE FORCE METHOD! \n");

      double *D = (double *)calloc(n*n, sizeof(double));

      add_multiply(D,A,B,n);
      //print_matrix(D, n);

      int errors = matrix_equal(C, D, n);
      printf("Errors: %d \n", errors);
  }
  
  // Clean up
  if (grid_rank == MASTER)
  {
    free(A);
    free(B);
    free(C);
  }

  free(a);
  free(b);
  free(c);
  free(a_temp);
  MPI_Type_free(&block_type);
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
