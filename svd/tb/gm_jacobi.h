// Anton Petrov
// jacobi.h - contains function declarations

typedef struct {
	double value;
	int rowNum;
	int colNum;
} LargestElement;

// Self-explanatory
void gm_multiply (double *left, double *right, double *result, int dimension);

// Function that performs a number of jacobi rotations on the input matrix a
// and returns the SVD of that matrix in the u, s and v matrices
void gm_jacobi (double *a, int n, double *s, double *u, double *v);

// Find largest off-diagonal element in input matrix. Uses the LargestElement
// struct to store the row, col and value of the element.
LargestElement gm_findLargestElement (double *matrix, int dimension, int **a11, int **a12, int **a21, int **a22);

// Self-explanatory
void gm_copyMatrix (double *a, double *b, int dimension);

// Loop over matrix and transform it into the identity matrix
void gm_identify (double *matrix, int dimension);

// Perform a single rotation
void gm_rotate (double *a, int dimension, double *u, double *v, int *x11, int *x12, int *x21, int *x22);

// Calculate the transpose of the input matrix in place
void gm_transpose (double *matrix, int dimension);

// Apply a series of permutation matrices to a so that the singular values
// are ordered in decreasing order. Apply those same permutations in the
// correct order to u and v.
void gm_reorder (double *a, int dimension, double *u, double *v);

// Swap two rows of a matric in place.
void gm_swapRows (double *m, int a, int b, int dimension);
