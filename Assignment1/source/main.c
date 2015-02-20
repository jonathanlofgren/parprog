#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MASTER 0
#define bl2 (blocksize*blocksize)

//#define DEBUG
//#define PRINT_MATRIX

void print_matrix(double *matrix, int n)
{
    int i, j;
  for (i=0; i<n; i++)
  {
      for (j=0; j<n; j++)
      {
	  printf("%0.8f ", matrix[i*n+j]);
      }
      printf("\n");
  }
  printf("\n");  
}


void print_part(double *matrix, int* disps, int nrows)
{
    int i, j;
  for (i=0; i<nrows; i++)
  {
      for (j=0; j<nrows; j++)
      {
	  printf("%0.0f ", matrix[disps[i*nrows+j]]);
      }
      printf("\n");
  }
  printf("\n");  
}


void print_matrix_int(int *matrix, int n)
{
    int i, j;
  for (i=0; i<n; i++)
  {
      for (j=0; j<n; j++)
      {
	  printf("%d ", matrix[i*n+j]);
      }
      printf("\n");
  }
  printf("\n");  
}


int matrix_equal(double *a, double *b, int n)
{
    int e = 0;
    int i;
    for (i = 0; i < n*n; i++)  
    {	
	if ((a[i] - b[i])*(a[i] - b[i]) > 0.1)
	{
	  //printf("%0.6f != %0.6f \n", a[i], b[i]);
	    e++;
	}      
    } 

   return e;
}


void add_multiply(double *c, double *a, double *b, int n)
{
    int i, j, k;
    for (i = 0; i < n; i++)  
    {	
	for (j = 0; j < n; j++)
	{
	    double sum = 0;

	    for (k = 0; k < n; k++) {
		sum += a[i*n+k] * b[k*n+j];
	    }

	    c[i*n+j] += sum;
	}
    }
}


int main(int argc, char *argv[]) 
{
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
      if (grid_rank == MASTER) 
      {
	  printf("Matrix size: %dx%d on %d processors\n", 
		 n, n, nproc);
      }
  }
    
  // Master process creates matrix A, B and C
  if (grid_rank == MASTER)
  {
    n=atoi(argv[1]);
    A=(double *)malloc(n*n*sizeof(double));
    B=(double *)malloc(n*n*sizeof(double));
    C=(double *)malloc(n*n*sizeof(double));

    srand((unsigned) time(NULL));
    
    for (int i=0; i<n*n; i++)
    {
      A[i]= ((double)rand()/(double)RAND_MAX);
      B[i]= ((double)rand()/(double)RAND_MAX);
      C[i]= 0;
    }

    #ifdef PRINT_MATRIX
    print_matrix(A,n);
    print_matrix(B,n);
    #endif
  }

  int coords[2], col_rank, row_rank;
  double t1, t2;
  MPI_Comm proc_row, proc_col;
  MPI_Status status;

  if (grid_rank == MASTER)
  {
      t1 = MPI_Wtime();
  }

  // Create communicators for row and column
  MPI_Cart_coords(proc_grid, grid_rank, ndim, coords); 
  MPI_Comm_split(proc_grid,coords[1],coords[0],&proc_col);
  MPI_Comm_split(proc_grid,coords[0],coords[1],&proc_row);
  MPI_Comm_rank(proc_col, &col_rank);
  MPI_Comm_rank(proc_row, &row_rank);

  // Allocate the blocks
  int blocksize = n / dims[0]; 
  double *a, *b, *c, *a_temp, *b_temp;
  a = (double *)calloc(bl2, sizeof(double));
  a_temp = (double *)calloc(bl2, sizeof(double));
  b = (double *)calloc(bl2, sizeof(double));
  b_temp = (double *)calloc(bl2, sizeof(double));
  c = (double *)calloc(bl2, sizeof(double));

  // Create datatype for the blocks
  MPI_Datatype block_type, block_type1;
  MPI_Type_vector(blocksize,blocksize,n,MPI_DOUBLE,&block_type1);
  MPI_Type_create_resized(block_type1, 0, sizeof(double), &block_type);
  MPI_Type_commit(&block_type);
  MPI_Barrier(proc_grid);
  
  // Create block positions for Scatter and Gather
  int disps[nproc];
  int counts[nproc];

  for (int i=0; i<dims[0]; i++) 
  {
      for (int j=0; j<dims[1]; j++) 
      {
	  disps[i*dims[1]+j] = i*n*blocksize+j*blocksize;
	  counts[i*dims[1]+j] = 1;
      }
  }
  
  #ifdef DEBUG
  if (grid_rank == MASTER)
  {
      printf("Counts:\n");
      print_matrix_int(&counts[0], dims[0]);
      printf("Disps:\n");
      print_matrix_int(&disps[0], dims[0]);
  }

  if (grid_rank == MASTER)
  {
      for(int i = 0; i < nproc; i++)
      {
	  B[disps[i]] = i;
	  B[disps[i]+(blocksize-1)*(n) + blocksize-1] = i;
	  printf("%d:%d\n", disps[i], disps[i]+(blocksize-1)*(n) + blocksize-1);
      }
  }
  #endif

  // Scatter block A and B
  MPI_Scatterv(A, counts, disps, block_type, a, bl2, MPI_DOUBLE, 0, proc_grid);
  MPI_Scatterv(B, counts, disps, block_type, b, bl2, MPI_DOUBLE, 0, proc_grid);

  int m, source, dest;
  MPI_Request request, request2;
  MPI_Status status2;

  memcpy(b_temp, b, bl2*sizeof(double));

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

    add_multiply(c,a_temp,b_temp,blocksize);
    
    MPI_Irecv(b_temp, bl2, MPI_DOUBLE, source, 43, proc_grid, &request2);
    
    MPI_Wait(&request, MPI_STATUS_IGNORE);
    MPI_Wait(&request2, MPI_STATUS_IGNORE);
    memcpy(b, b_temp, bl2*sizeof(double));
    
    #ifdef DEBUG
    if (row_rank == 2)
    {
	printf("Grid_rank: %d, source: %d, dest: %d,"
	       "  b[0] = %0.3f, b[sista] = %0.3f\n", 
	       grid_rank, source, dest, b[0],b[bl2-1]);
    }
    #endif
  }

  // Gather all c blocks to MASTER 
  MPI_Gatherv(c, bl2, MPI_DOUBLE, C, counts, disps, block_type, 0, proc_grid);
  
  // Print calculation wall time
  if (grid_rank == MASTER)
  {
      t2 = MPI_Wtime();
      printf("Total time: %0.3f seconds.\n", t2-t1);
  }

  #ifdef DEBUG
  if (grid_rank == MASTER)
  {
      #ifdef PRINT_MATRIX
      printf("Result from fox:\n");
      print_matrix(C, n); 
      #endif

      double *D = (double *)calloc(n*n, sizeof(double));
      add_multiply(D,A,B,n);
      
      #ifdef PRINT_MATRIX
      print_part(C,disps,dims[0]);
      print_part(D,disps,dims[0]);
      #endif

      int errors = matrix_equal(C, D, n);
      printf("Errors: %d \n", errors);
      free(D);
  }
  #endif

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
  free(b_temp);
  MPI_Type_free(&block_type);
  MPI_Finalize();
  return 0;
}
