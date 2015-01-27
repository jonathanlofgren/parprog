#include "matrix.h"

int main()
{
    matrix_t matrix;
    int n;
    double max_norm;

    // Prompt user and read matrix size from stdin.
    printf("Enter the Dimension for a square matrix: ");
    scanf("%d",n);
    printf("\n");
    
    matrix_allocate(&matrix, n);

    // Fill matrix.
    matrix_fill_random(matrix);

    // Compute max norm of matrix and print it.
    max_norm = matrix_max_norm(matrix);
    matrix_deallocate(&matrix);
    printf("Maximum norm : %.4f\n", max_norm);
    
    // Print matrix.
    matrix_print(matrix);

    // Deallocate matrix.    
    matrix_deallocate(&matrix);
    
    // Exit.
    return 0;
}

