#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 *  Type representing a square matrix of size n x n.
 */
typedef struct {
    double **data;
    int n;
} matrix_t;

/** 
 *  Function prototypes.
 */
void matrix_allocate(matrix_t *matrix, int n);
void matrix_deallocate(matrix_t *matrix);
void matrix_fill_random(matrix_t matrix);
void matrix_print(matrix_t matrix);
double matrix_max_norm(matrix_t matrix);

