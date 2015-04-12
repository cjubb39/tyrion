#include "svd.h"

/* TODO generalize to CELL_TYPE */
double inline fp_abs(double in) {
	return (in > 0) ? in : -in;
}

void svd::identify (double *matrix, int dimension) {
	int row, col;
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;
		for (col = 0; col < MAX_SIZE; col++) {
			if (col == dimension) break;
			// Elements on diagonal =1
			if (col == row) {matrix [(row * dimension) + col] = 1.0;}
			// ... all others 0
			else {matrix [(row * dimension) + col] = 0.0;}
		}
	}
	return;
}

void svd::copyMatrix (double *a, double *b, int dimension) {
	int row, col;
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;
		for (col = 0; col < MAX_SIZE; col++) {
			if (col == dimension) break;
			b [(row * dimension) + col] = a [(row * dimension) + col];
		}
	}
	return;
}

void svd::multiply (double *left, double *right, double *result, int dimension) {
	int leftRow, rightRow; // row numbers of the left and right matrix, respectively	
	int leftCol, rightCol; // same as above but for columns
	double tempResult = 0;

	for (leftRow = 0; leftRow < MAX_SIZE; leftRow++) {
		if (leftRow == dimension) break;
		for (rightCol = 0; rightCol < MAX_SIZE; rightCol++) {
			if (rightCol == dimension) break;

			for (leftCol = rightRow = 0; leftCol < MAX_SIZE;
					leftCol++, rightRow++) {
				if (leftCol == dimension) break;
				tempResult += left[(leftRow * dimension) + leftCol] * right[(rightRow * dimension) + rightCol];
			}
			result [(leftRow * dimension) + rightCol] = tempResult;
			tempResult = 0;
		}
	}
	return;
}

void svd::jacobi (double *a, int n, double *s, double *u, double *v) {
	// Arrays that contain the coordinates of the elements of the 2x2
	// sub matrix formed by the largest off-diagonal element. First entry
	// is the row number and second entry is the column number.
	int a11[2];
	int a12[2];
	int a21[2];
	int a22[2];
	LargestElement le; // largest element of matrix a
	le.value = le.rowNum = le.colNum = -1;
	int i, j;

	// I could have statically allocated these but I would not have been
	// able to pass them as double pointers: this is a known problem in C
	fprintf(stderr, "malloc first run complete\n");

	identify (u, n);
	fprintf(stderr, "id1 run complete\n");
	identify (v, n);
	fprintf(stderr, "id2 run complete\n");

	if (n == 1) {	// 1x1 matrix is already in SVD
		s[0] = a[0];
		return;
	}

	le = findLargestElement (a, n, a11, a12, a21, a22);
	fprintf(stderr, "fle 1 complete\n");

	int count = 0;
	/* FIXME This loop may not be synthesizable */
	while (fp_abs (le.value) > 0.00000000000000000001) {
		count++;
		rotate (a, n, u, v, a11, a12, a21, a22);
		le = findLargestElement (a, n, a11, a12, a21, a22);
	}

	reorder (a, n, u, v);
	fprintf(stderr, "reorder complete\n");

	// Copy over the singular values in a to s
	for (i = 0; i < MAX_SIZE; i++) {
		if (i == n) break;
		j = i;
		s [i] = a [(i * n) + j];
	}

	fprintf(stderr, "just free left\n");
	fprintf(stderr, "free complete\n");
	return;
}

void svd::rotate (double *a, int dimension, double *u, double *v, int *x11, int *x12, int *x21, int *x22) {
	double a11, a12, a21, a22; // elements of the sub-matrix
	double alpha, beta; // angles used in rotations; alpha is angle of left rotation and beta is angle of right rotation
	double cosA, sinA, cosB, sinB;
	double X, Y; // temporary values used in calculating angles

	// Assign elements of sub-matrix to the their actual values from a
	a11 = a [x11[0] * dimension + x11[1]]; a12 = a [x12[0] * dimension + x12[1]];
	a21 = a [x21[0] * dimension + x21[1]]; a22 = a [x22[0] * dimension + x22[1]];

	// Calculate angles and sin and cos of those angles using the closed
	// formulas found.
	/* FIXME trig functions */
	X = atan ((a21 - a12) / (a11 + a22)); Y = atan ((a21 + a12) / (a11 - a22));
	alpha = 0.5 * (X + Y);
	beta = 0.5 * (Y - X);
	/* FIXME trig functions */
	cosA = cos (alpha); sinA = sin (alpha);
	cosB = cos (beta); sinB = sin (beta);

	// Create left rotation matrix, namely U_i which looks like this
	//
	//		| cosA	sinA |
	//		| -sinA	cosA |
	//
	/* Ui statically created */
	identify (Ui, dimension);
	Ui [x11[0] * dimension + x11[1]] = Ui [x22[0] * dimension + x22[1]] = cosA;
	Ui [x12[0] * dimension + x12[1]] = sinA;
	Ui [x21[0] * dimension + x21[1]] = (-1.0) * sinA;

	// Create the right rotation matrix, namely V_i which looks like this
	//
	//		| cosB	-sinB |
	//		| sinB	 cosB |
	//
	/* Vi statically created */
	identify (Vi, dimension);
	Vi [x11[0] * dimension + x11[1]] = Vi [x22[0] * dimension + x22[1]] = cosB;
	Vi [x12[0] * dimension + x12[1]] = (-1.0) * sinB;
	Vi [x21[0] * dimension + x21[1]] = sinB;

	// Rotate a
	// First on the left with Ui
	/* tempResult statically created */
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

	return; 
}

LargestElement svd::findLargestElement (double *matrix, int dimension, int *a11, int *a12, int *a21, int *a22) {
	LargestElement leTemp;
	int i, j;

	leTemp.value = 0;
	leTemp.rowNum = leTemp.colNum = -1;

	// Populate leArray such that the entry at index i contains information
	// about the largest element of row i
	for (i = 0; i < MAX_SIZE; i++) {
		if (i == dimension) break;
		for (j = 0; j < MAX_SIZE; j++) {
			if (j == dimension) break;
			// We are looking for the largest OFF-DIAGONAL element
			if (j == i)
				continue;
			if (fp_abs (matrix [(i * dimension) + j]) > fp_abs (leTemp.value)) {
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
	for (i = 0; i < MAX_SIZE; i++) {
		if (i == dimension) break;
		if (fp_abs (leArray [i].value) > fp_abs (leTemp.value)) {
			leTemp.value = leArray [i].value;
			leTemp.rowNum = leArray [i].rowNum;
			leTemp.colNum = leArray [i].colNum;
		}
	}

	// Determining coordinates of the 2x2 sub matrix formed by this largest
	// element
	if (leTemp.rowNum < leTemp.colNum) { // largest element is above diagonal
		(a11) [0] = (a11) [1] = (a12) [0] = (a21) [1] = leTemp.rowNum;
		(a21) [0] = (a22) [0] = (a22) [1] = (a12) [1] = leTemp.colNum;	
	}
	else { // below diagonal
		a22[0] = leTemp.rowNum;
		a22[1] = leTemp.rowNum;
		a21[0] = leTemp.rowNum;
		a12[1] = leTemp.rowNum;

		a11[0] = leTemp.colNum;
		a11[1] = leTemp.colNum;
		a21[1] = leTemp.colNum;
		a12[0] = leTemp.colNum;
	}

	return leTemp;
}

void svd::transpose (double *matrix, int dimension) {
	double temp;
	int row, col;
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;
		for (col = (row + 1); col < MAX_SIZE; col++) {
			if (col == dimension) break;
			if (col == row) {continue;} // skip diagonal elements
			temp = matrix [(row * dimension) + col];
			matrix [(row * dimension) + col] = matrix [(col * dimension) + row];
			matrix [(col * dimension) + row] = temp;
		}
	}
	return;
}

void svd::reorder (double *a, int dimension, double *u, double *v) {
	int row, col, x, largestElementRow;
	double temp; // stores the largest singular value

	/* p statically allocated */
	/* tempMatrix statically allocated */

	for (x = 0; x < MAX_SIZE; x++) {
		if (x == dimension) break;
		temp = 0.0;
		identify (p, dimension);
		for (row = x; row < MAX_SIZE; row++) {
			if (row == dimension) break;
			col = row;
			if (fp_abs (a [row * dimension + col]) > fp_abs (temp)) {
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
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;
		col = row;
		if (a [row * dimension + col] < 0)
			p [row * dimension + col] = -1.0;
	}
	multiply (p, a, tempMatrix, dimension);
	copyMatrix (tempMatrix, a, dimension);
	multiply (u, p, tempMatrix, dimension);
	copyMatrix (tempMatrix, u, dimension);
	return;
}

// Swap row a with row b of matrix m
void svd::swapRows (double *m, int a, int b, int dimension) {
	double temp;
	for (int col = 0; col < MAX_SIZE; col++) {
		if (col == dimension) break;
		temp = m [a * dimension + col];
		m [a * dimension + col] = m [b * dimension + col];
		m [b * dimension + col] = temp;
	}
	return;
}


void svd::config_svd()
{
	// Initialization
	size.write(0);
	init_done.write(false);

	wait();

	// Read configuration until done
	bool done = false;
CONFIG_REGISTER_WHILE:
	do {
		wait();
		done = conf_done.read();
		int a = conf_size.read();
		size.write(a);
	} while (!done);

	// Let other threads run then do nothing
	init_done.write(true);
	while (true) {
		wait();
	}
}

void svd::load_input()
{
RESET_LOAD:
	bufdin.reset_get();

	rd_index.write(0);
	rd_length.write(0);
	rd_request.write(false);

	input_done.write(false);

	do {wait();}
	while(!init_done.read());

	const int rows = size.read();
	int index = 0;

LOAD_INPUT_WHILE:
	while(true) {
		if (index == rows * rows)
			// Input complete; wait for reset
			do { wait(); }	while (true);

		int length = rows * rows;

		rd_index.write(index);
		rd_length.write(length);
		index += length;

		// 4-phase handshake
		rd_request.write(true);
		do { wait(); } while(!rd_grant.read());
		rd_request.write(false);
		do { wait(); } while(rd_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = bufdin.get();
				matrix_in[i * rows + j] = cell;
#ifdef VERBOSE
				cout << "DUT GET: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}

		// 4-phase handshake
		cout << "DUT process start handshake starting" << endl;
		input_done.write(true);
		do { wait(); } while (!process_start.read());
		input_done.write(false);
		do { wait(); } while (process_start.read());
		cout << "DUT process start handshake complete" << endl;
	}
}

void svd::store_output()
{
RESET_STORE:
	bufdout.reset_put();

	wr_index.write(0);
	wr_request.write(false);
	wr_length.write(0);

	output_start.write(false);
	svd_done.write(false);

	do {wait();}
	while(!init_done.read());

	const int rows = size.read();
	int length = rows * rows;
	int index = 0;

STORE_OUTPUT_WHILE:
	while(true) {

		if (index == NUM_OUTPUT_MATRIX * rows * rows) {
			// DEBAYER Done (need a reset)
			cout << "DUT DEBAYER DONE" << endl;
			svd_done.write(true);
			do { wait(); } while(true);
		}

		// 4-phase handshake
		do { wait(); }
		while (!process_done.read());
		output_start.write(true);
		do { wait(); }
		while (process_done.read());
		output_start.write(false);

		/* S matrix */
		// Send DMA request
		wr_index.write(index);
		wr_length.write(length);
		index += length;

		wr_request.write(true);
		do { wait(); } while(!wr_grant.read());
		wr_request.write(false);
		do { wait(); } while(wr_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = s[i * size + j];
				bufdout.put(cell);
#ifdef VERBOSE
				cout << "DUT PUT: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}
		

		/* U matrix */
		// Send DMA request
		wr_index.write(index);
		wr_length.write(length);
		index += length;

		wr_request.write(true);
		do { wait(); } while(!wr_grant.read());
		wr_request.write(false);
		do { wait(); } while(wr_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = u[i * size + j];
				bufdout.put(cell);
#ifdef VERBOSE
				cout << "DUT PUT: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}
		

		/* V matrix */
		// Send DMA request
		wr_index.write(index);
		wr_length.write(length);
		index += length;

		wr_request.write(true);
		do { wait(); } while(!wr_grant.read());
		wr_request.write(false);
		do { wait(); } while(wr_grant.read());

		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
			for (int j = 0; j < MAX_SIZE; ++j) {
				if (j == rows) break;
				SVD_CELL_TYPE cell = v[i * size + j];
				bufdout.put(cell);
#ifdef VERBOSE
				cout << "DUT PUT: (index, length, i, j, val)" << index << " " << length
					<< " " << i << " " << j << " " << cell << endl;
#endif
				wait();
			}
		}
	}
}

void svd::process_svd()
{
	// Reset
	process_start.write(false);
	process_done.write(false);

	// Wait for configuratin
	do { wait(); } while (!init_done.read());

	const int rows = size.read();
	int svd_row = 0;

DEBAYER_WHILE:
	while (true) {

		if (svd_row == rows)
			// Wait for reset
			do { wait(); } while(true);

		// 4-phase handshake
		do { wait(); }
		while (!input_done.read());
		process_start.write(true);
		do { wait(); }
		while (input_done.read());
		process_start.write(false);
		cout << "DUT process start handshake complete: PS" << endl;

		cout << "about to jacobi start" << endl;
		jacobi(matrix_in, rows, s, u, v);
		cout << "jacobi finish" << endl;

		// 4-phase handshake
		process_done.write(true);
		do { wait(); }
		while (!output_start.read());
		process_done.write(false);
		do { wait(); }
		while (output_start.read());

		svd_row += rows;
	}
}

