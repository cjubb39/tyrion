// Anton Petrov

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "getLine.h"
#include "jacobi.h"

typedef struct {
	int dimension; 
	FILE *matrixFilePtr;
} MatrixInfo;

void generateMatrix (MatrixInfo *matrixInfo, char *suffix, int numMatrices, double (*element) (int, int));
void generateMatrices (void);

int main (int argc, const char *argv[]) {
	int rowNum, colNum;
	FILE *fPtr = NULL; // file pointer to file containing input matrix
	char *matrixRow = NULL; // pointer to a row of the input matrix
	double *matrix = NULL; // input matrix stored in a single array of doubles
	int dimension = 0; // dimension of input matrix
	bool firstLine = true; // is the current line the first one?
	char *element = NULL; // single element of input matrix
	double *u, *s, *v; // matrices used in SVD
	double *tempResult, *tempResult2;

	// Generate testing matrices
	if (argv [1] != NULL)
		generateMatrices ();
	// Read in input matrix from file called "input-matrix.txt"
	fPtr = fopen ("input-matrix.txt", "r");
	for (rowNum = 0; (matrixRow = getLine (fPtr)) != NULL; rowNum++) {
		// First line, by convention always specifies the dimension of the
		// matrx
		if (firstLine) {
			dimension = atoi (matrixRow);
			matrix = (double *) malloc (sizeof (double)*(dimension*dimension));
			firstLine = false;
			rowNum -= 1; //because this line wasn't an actual row of the matrix
			free (matrixRow);
			continue;
		}
		// Tokenize input line by tab characters which by convention are used
		// to separate the elements of the input matrix.
		element = strtok (matrixRow, "\t");
		for (colNum = 0; element != NULL; colNum++) {
			matrix[(rowNum * dimension) + colNum] = atof (element);
			element = strtok (NULL, "\t");
		}
		free (matrixRow);
	}
	u = (double *) malloc (sizeof (double) * (dimension * dimension));	
	s = (double *) malloc (sizeof (double) * dimension);
	v = (double *) malloc (sizeof (double) * (dimension * dimension));
	tempResult = (double *) malloc (sizeof (double) * (dimension * dimension));
	tempResult2 = (double *) malloc (sizeof (double) * (dimension * dimension));

	jacobi (matrix, dimension, s, u, v);

	// Printing results
#if 0
	printf ("\nSingular matrix -- A");
	printMatrix (matrix, dimension);
	printf ("\nLeft Orthogonal -- U");
	printMatrix (u, dimension);
	printf ("\nRight Orthogonal -- Vt");
	printMatrix (v, dimension);
	printf ("\nSingular values as stored in S\n");
#endif
	for (rowNum = 0; rowNum < dimension; rowNum++)
		printf ("%f\n", s[rowNum]);
#if 0
	printf ("\n\n");
	printf ("Check that U * A * Vt multiply back together\n");
	multiply (u, matrix, tempResult, dimension);
	multiply (tempResult, v, tempResult2, dimension);
	printMatrix (tempResult2, dimension);
	printf ("\nCheck that U and Vt are orthogonal\n");
	printf ("U * Ut");
	tempResult2 [0] = 99.0; // make sure that tempResult2 actually changes
	copyMatrix (u, tempResult, dimension);
	transpose (tempResult, dimension);
	multiply (u, tempResult, tempResult2, dimension);
	printMatrix (tempResult2, dimension);
	printf ("V * Vt \n");
	tempResult2 [0] = 99.0;
	copyMatrix (v, tempResult, dimension);
	transpose (tempResult, dimension);
	multiply (tempResult, v, tempResult2, dimension);
	printMatrix (tempResult2, dimension);
#endif


	free (u); free (v); free (s); free (matrix); free(tempResult);
	fclose(fPtr); free (tempResult2);
	return EXIT_SUCCESS;
}

double formula2 (int i, int j) {
	return (i*i) + (j*j);
}

double formula1 (int i, int j) {
	return sqrt ((i*i) + (j*j));
}	

void generateMatrix (MatrixInfo *matrixInfo, char *suffix, int numMatrices, double (*element) (int, int)) {
	int i, j, k;
	char fileName [80];
	for (k = 0; k < numMatrices; k++) {
		sprintf (fileName, "%d", matrixInfo[k].dimension);
		matrixInfo[k].matrixFilePtr = fopen (strcat (fileName, suffix), "w");
		for (i = 1; i <= matrixInfo[k].dimension; i++) {
			if (i == 1) {fprintf (matrixInfo[k].matrixFilePtr, "%d\n", (k + 1) *10);}
			for (j = 1; j <= matrixInfo[k].dimension; j++) {
				if (j < matrixInfo[k].dimension)
					fprintf (matrixInfo[k].matrixFilePtr, "%f\t", element (i, j));
				else
					fprintf (matrixInfo[k].matrixFilePtr, "%f\n", element (i, j));
			}
		}
		fclose (matrixInfo[k].matrixFilePtr);
	}
}

// Generate test matrices using formulas specified in problem set. Write these
// matrices to file. If such a file already exists, overwrite the previous
// contents.
void generateMatrices (void) {

	int numMatrices;
	MatrixInfo *matrixInfo;

	// Generate 3 matrices using the formula  A [i][j] = sqrt (i*i + j*j)
	numMatrices = 3;
	matrixInfo = (MatrixInfo *) malloc (sizeof (MatrixInfo) * numMatrices);
	matrixInfo[0].dimension = 10;
	matrixInfo[1].dimension = 20;
	matrixInfo[2].dimension = 40;
	generateMatrix (matrixInfo, "-sqrt.txt", numMatrices, &formula1);
	free (matrixInfo);

	// Generate 1 matrix using the formula A[i][j] = (i * i) + (j * j)
	numMatrices = 11;
	matrixInfo = (MatrixInfo *) malloc (sizeof (MatrixInfo) * numMatrices);
	matrixInfo[0].dimension = 10;
	matrixInfo[1].dimension = 15;
	matrixInfo[2].dimension = 20;
	matrixInfo[3].dimension = 25;
	matrixInfo[4].dimension = 30;
	matrixInfo[5].dimension = 35;
	matrixInfo[6].dimension = 40;
	matrixInfo[7].dimension = 45;
	matrixInfo[8].dimension = 50;
	matrixInfo[9].dimension = 55;
	matrixInfo[10].dimension = 60;
	generateMatrix (matrixInfo, "-square_and_add.txt", numMatrices, &formula2);
	free (matrixInfo);

	return;
}
