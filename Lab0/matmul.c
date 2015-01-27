#include <stdlib.h>
#include <stdio.h>

typedef struct matrix{  

  int **m;
  int row,col;  
  
} Matrix;

Matrix new_matrix(int, int);
void set_matrix(Matrix, int);
void free_matrix(Matrix);
Matrix multiply(Matrix,Matrix);
void print_matrix(Matrix);

int main(void){

  Matrix a = new_matrix(2, 3);
  Matrix b = new_matrix(3, 5);

  set_matrix(a, 1);
  set_matrix(b, 2);

  Matrix c = multiply(a,b);

  print_matrix(a);
  print_matrix(b);
  print_matrix(c);
  
  free_matrix(a);
  free_matrix(b);
  free_matrix(c);

  return 0;
}

Matrix new_matrix(int row, int col){
  Matrix a;
  a.row = row;
  a.col = col;
  
  int* data = (int *)calloc(row*col, sizeof(int));
  a.m = (int **)malloc(row*sizeof(int*));

  int i;
  for (i = 0; i < row; i++) {
    a.m[i] = &data[i*col];
  } 
  
  return a;
}

void free_matrix(Matrix a) {
  int i;
  
  for (i = 0; i < a.row; i++) {
    free(a.m[i]);
  }
  
  free(a.m);
}

Matrix multiply(Matrix a, Matrix b){
  Matrix res;

  if (a.col != b.row){
    printf("Fel! \n");
    return res;
  }  

  int n = a.col;
  
  res = new_matrix(a.row, b.col);

  int i,j,k;
  for(i = 0; i < res.row; i++){
    for(j=0; j < res.col; j++){
      int sum = 0;

      for (k = 0; k < n; k++) {
	sum += a.m[i][k] * b.m[k][j];
      }
      
      res.m[i][j] = sum; 
    }
  }

  return res;
}

void print_matrix(Matrix a){
  printf("%d x %d \n", a.row, a.col);

  int i,j;
  for (i=0; i<a.row; i++){
    for (j=0; j<a.col; j++){
      printf("%3d", a.m[i][j]);
    }
    printf("\n");
  }
  
  printf("\n");
}

void set_matrix(Matrix a, int val){

  int i,j;
  for (i=0; i<a.row; i++){
    for (j=0; j<a.col; j++){
      a.m[i][j] = val;
    }
  }

}
