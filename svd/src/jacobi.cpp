// CPSC 445 - Problem Set 1
// Anton Petrov 22-09-11

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "jacobi.h"

void identify (double *matrix, int dimension) {
	int row, col;
	for (row = 0; row < dimension; row++) {
		for (col = 0; col < dimension; col++) {
			// Elements on diagonal =1
			if (col == row) {matrix [(row * dimension) + col] = 1.0;}
			// ... all others 0
			else {matrix [(row * dimension) + col] = 0.0;}
		}
	}
	return;
}

void copyMatrix (double *a, double *b, int dimension) {
	int row, col;
	for (row = 0; row < dimension; row++) {
		for (col = 0; col < dimension; col++)
			b [(row * dimension) + col] = a [(row * dimension) + col];
	}
	return;
}

void multiply (double *left, double *right, double *result, int dimension) {
	int leftRow, rightRow; // row numbers of the left and right matrix, respectively	
	int leftCol, rightCol; // same as above but for columns
	double tempResult = 0;

	for (leftRow = 0; leftRow < dimension; leftRow++) {
		for (rightCol = 0; rightCol < dimension; rightCol++) {
			for (leftCol = rightRow = 0; leftCol < dimension; leftCol++, rightRow++)
				tempResult += left[(leftRow * dimension) + leftCol] * right[(rightRow * dimension) + rightCol];
			result [(leftRow * dimension) + rightCol] = tempResult;
			tempResult = 0;
		}
	}
	return;
}

void jacobi (double *a, int n, double *s, double *u, double *v) {
	// Arrays that contain the coordinates of the elements of the 2x2
	// sub matrix formed by the largest off-diagonal element. First entry
	// is the row number and second entry is the column number.
	int *a11, *a12, *a21, *a22;
	LargestElement le; // largest element of matrix a
	le.value = le.rowNum = le.colNum = -1;
	int i, j;

	// I could have statically allocated these but I would not have been
	// able to pass them as double pointers: this is a known problem in C
	a11=(int*)malloc(sizeof(int)*2); a12=(int*)malloc(sizeof(int)*2);
	a21=(int*)malloc(sizeof(int)*2); a22=(int*)malloc(sizeof(int)*2);
	fprintf(stderr, "malloc first run complete\n");

	identify (u, n);
	fprintf(stderr, "id1 run complete\n");
	identify (v, n);
	fprintf(stderr, "id2 run complete\n");

	if (n == 1) {	// 1x1 matrix is already in SVD
		s[0] = a[0];
		return;
	}

	le = findLargestElement (a, n, &a11, &a12, &a21, &a22);
	fprintf(stderr, "fle 1 complete\n");

	int count = 0;
	while (fabs (le.value) > 0.00000000000000000001) {
		count++;
		rotate (a, n, u, v, a11, a12, a21, a22);
		le = findLargestElement (a, n, &a11, &a12, &a21, &a22);
	}

	reorder (a, n, u, v);
	fprintf(stderr, "reorder complete\n");

	// Copy over the singular values in a to s
	for (i = 0; i < n; i++) {
		j = i;
		s [i] = a [(i * n) + j];
	}

	fprintf(stderr, "just free left\n");
	free (a11); free (a12); free (a21); free (a22);
	fprintf(stderr, "free complete\n");
	return;
}

void rotate (double *a, int dimension, double *u, double *v, int *x11, int *x12, int *x21, int *x22) {
	double a11, a12, a21, a22; // elements of the sub-matrix
	double alpha, beta; // angles used in rotations; alpha is angle of left rotation and beta is angle of right rotation
	double cosA, sinA, cosB, sinB;
	double *Ui, *Vi; // left and right rotation matrices, respectively
	double *tempResult;
	double X, Y; // temporary values used in calculating angles

	// Assign elements of sub-matrix to the their actual values from a
	a11 = a [x11[0] * dimension + x11[1]]; a12 = a [x12[0] * dimension + x12[1]];
	a21 = a [x21[0] * dimension + x21[1]]; a22 = a [x22[0] * dimension + x22[1]];

	// Calculate angles and sin and cos of those angles using the closed
	// formulas found.
	X = atan ((a21 - a12) / (a11 + a22)); Y = atan ((a21 + a12) / (a11 - a22));
	alpha = 0.5 * (X + Y);
	beta = 0.5 * (Y - X);
	cosA = cos (alpha); sinA = sin (alpha);
	cosB = cos (beta); sinB = sin (beta);

	// Create left rotation matrix, namely U_i which looks like this
	//
	//		| cosA	sinA |
	//		| -sinA	cosA |
	//
	Ui = (double *) malloc (sizeof (double) * (dimension * dimension));
	identify (Ui, dimension);
	Ui [x11[0] * dimension + x11[1]] = Ui [x22[0] * dimension + x22[1]] = cosA;
	Ui [x12[0] * dimension + x12[1]] = sinA;
	Ui [x21[0] * dimension + x21[1]] = (-1.0) * sinA;

	// Create the right rotation matrix, namely V_i which looks like this
	//
	//		| cosB	-sinB |
	//		| sinB	 cosB |
	//
	Vi = (double *) malloc (sizeof (double) * (dimension * dimension));
	identify (Vi, dimension);
	Vi [x11[0] * dimension + x11[1]] = Vi [x22[0] * dimension + x22[1]] = cosB;
	Vi [x12[0] * dimension + x12[1]] = (-1.0) * sinB;
	Vi [x21[0] * dimension + x21[1]] = sinB;

	// Rotate a
	// First on the left with Ui
	tempResult = (double *) malloc (sizeof (double) * (dimension * dimension));
	multiply (Ui, a, tempResult, dimension);
	copyMatrix (tempResult, a, dimension);
	// Then on the right with Vi
	multiply (a, Vi, tempResult, dimension);
	copyMatrix (tempResult, a, dimension);

	// Apply rotation matrix Ui to U
	multiply (Ui, u, tempResult, dimension);
	copyMatrix (tempResult, u, dimension);
	// Apply rotation matrix Vi to V
	multiply (v, Vi, tempResult, dimension);
	copyMatrix (tempResult, v, dimension);

	// Free rotation matrices
	free (Ui); free (Vi); free (tempResult);
	return; 
}

LargestElement findLargestElement (double *matrix, int dimension, int **a11, int **a12, int **a21, int **a22) {
	LargestElement *leArray;
	LargestElement leTemp;
	int i, j;

	leTemp.value = 0;
	leTemp.rowNum = leTemp.colNum = -1;

	leArray = (LargestElement *) malloc (sizeof (LargestElement) * dimension);

	// Populate leArray such that the entry at index i contains information
	// about the largest element of row i
	for (i = 0; i < dimension; i++) {
		for (j = 0; j < dimension; j++) {
			// We are looking for the largest OFF-DIAGONAL element
			if (j == i)
				continue;
			if (fabs (matrix [(i * dimension) + j]) > fabs (leTemp.value)) {
				leTemp.value = matrix[(i * dimension) + j];
				leTemp.rowNum = i;
				leTemp.colNum = j;
			}
		}
		leArray[i].value = leTemp.value;
		leArray[i].rowNum = leTemp.rowNum;
		leArray[i].colNum = leTemp.colNum;
		leTemp.value = 0;
	}
	leTemp.value = 0;
	leTemp.rowNum = leTemp.colNum = -1;
	// Iterate over leArray and find the largest element in the matrix as a
	// whole
	for (i = 0; i < dimension; i++) {
		if (fabs (leArray [i].value) > fabs (leTemp.value)) {
			leTemp.value = leArray [i].value;
			leTemp.rowNum = leArray [i].rowNum;
			leTemp.colNum = leArray [i].colNum;
		}
	}

	free (leArray);

	// Determining coordinates of the 2x2 sub matrix formed by this largest
	// element
	if (leTemp.rowNum < leTemp.colNum) { // largest element is above diagonal
		(*a11) [0] = (*a11) [1] = (*a12) [0] = (*a21) [1] = leTemp.rowNum;
		(*a21) [0] = (*a22) [0] = (*a22) [1] = (*a12) [1] = leTemp.colNum;	
	}
	else { // below diagonal
		(*a22) [0] = (*a22) [1] = (*a21) [0] = (*a12) [1] = leTemp.rowNum;
		(*a11) [0] = (*a11) [1] = (*a21) [1] = (*a12) [0] = leTemp.colNum;
	}

	return leTemp;
}

void transpose (double *matrix, int dimension) {
	double temp;
	int row, col;
	for (row = 0; row < dimension; row++) {
		for (col = (row + 1); col < dimension; col++) {
			if (col == row) {continue;} // skip diagonal elements
			temp = matrix [(row * dimension) + col];
			matrix [(row * dimension) + col] = matrix [(col * dimension) + row];
			matrix [(col * dimension) + row] = temp;
		}
	}
	return;
}

void reorder (double *a, int dimension, double *u, double *v) {
	int row, col, x, largestElementRow;
	double temp; // stores the largest singular value
	double *p; // permutation matrix
	double *tempMatrix;

	p = (double *) malloc (sizeof (double) * (dimension * dimension));
	tempMatrix = (double *) malloc (sizeof (double) * (dimension * dimension));

	for (x = 0; x < dimension; x++) {
		temp = 0.0;
		identify (p, dimension);
		for (row = x; row < dimension; row++) {
			col = row;
			if (fabs (a [row * dimension + col]) > fabs (temp)) {
				temp = a [row * dimension + col];
				largestElementRow = row;
			}
		}
		swapRows (p, x, largestElementRow, dimension);
		// Reorder a
		multiply (p, a, tempMatrix, dimension); copyMatrix (tempMatrix, a, dimension);
		multiply (a, p, tempMatrix, dimension); copyMatrix (tempMatrix, a, dimension);
		// Reorder u
		multiply (p, u, tempMatrix, dimension); copyMatrix (tempMatrix, u, dimension);
		// Reorder v
			multiply (v, p, tempMatrix, dimension); copyMatrix (tempMatrix, v, dimension);
	}

	transpose (u, dimension);
	transpose (v, dimension);
	identify (p, dimension);
	for (row = 0; row < dimension; row++) {
		col = row;
		if (a [row * dimension + col] < 0)
			p [row * dimension + col] = -1.0;
	}
	multiply (p, a, tempMatrix, dimension);
	copyMatrix (tempMatrix, a, dimension);
	multiply (u, p, tempMatrix, dimension);
	copyMatrix (tempMatrix, u, dimension);
	free (p); free (tempMatrix);
	return;
}

// Swap row a with row b of matrix m
void swapRows (double *m, int a, int b, int dimension) {
	double temp;
	for (int col = 0; col < dimension; col++) {
		temp = m [a * dimension + col];
		m [a * dimension + col] = m [b * dimension + col];
		m [b * dimension + col] = temp;
	}
	return;
}
