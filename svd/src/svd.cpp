#include "svd.h"

#ifdef CTOS_SC_FIXED_POINT
#include "svd_ctos_funcs.h"
#endif

SVD_CELL_TYPE inline fp_abs(SVD_CELL_TYPE in) {
#ifdef CTOS_SC_FIXED_POINT
	return sld::abs(in);
#else
	if (in >= 0)
		return in;
	else
		return -in;
#endif
}

void svd::identify (SVD_CELL_TYPE *matrix, int dimension) {
	int row, col;
IDENTITY_OUTER:
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;
IDENTITY_INNER:
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

void svd::copyMatrix (SVD_CELL_TYPE *a, SVD_CELL_TYPE *b, int dimension) {
	int row, col;

COPY_MATRIX_OUTER:
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;

COPY_MATRIX_INNER:
		for (col = 0; col < MAX_SIZE; col++) {
			if (col == dimension) break;
			b [(row * dimension) + col] = a [(row * dimension) + col];
		}
	}
	return;
}

void svd::multiply (SVD_CELL_TYPE *left, SVD_CELL_TYPE *right, SVD_CELL_TYPE *result, int dimension) {
	int leftRow, rightRow; // row numbers of the left and right matrix, respectively	
	int leftCol, rightCol; // same as above but for columns
	SVD_CELL_TYPE tempResult = 0;

MULTIPLY_MATRIX_OUTER:
	for (leftRow = 0; leftRow < MAX_SIZE; leftRow++) {
		if (leftRow == dimension) break;

MULTIPLY_MATRIX_MID:
		for (rightCol = 0; rightCol < MAX_SIZE; rightCol++) {
			if (rightCol == dimension) break;

MULTIPLY_MATRIX_INNER:
			for (leftCol = rightRow = 0; leftCol < MAX_SIZE;
					leftCol++, rightRow++) {
				if (leftCol == dimension) break;
				/* TODO reevaluate best way to handle this re: waits */
				SVD_CELL_TYPE tmp, tmp2;
				tmp = left[(leftRow * dimension) + leftCol];
				wait();
				tmp2 = right[(rightRow * dimension) + rightCol];
				tempResult += tmp * tmp2;
				wait();
			}
			result [(leftRow * dimension) + rightCol] = tempResult;
			tempResult = 0;
			wait();
		}
	}
	return;
}

void svd::jacobi (SVD_CELL_TYPE *a, int n, SVD_CELL_TYPE *s, SVD_CELL_TYPE *u, SVD_CELL_TYPE *v) {
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

	identify (u, n);
	identify (v, n);

	if (n == 1) {	// 1x1 matrix is already in SVD
		SVD_CELL_TYPE tmp = s[0];
		wait();
		s[0] = tmp;
		return;
	}

	le = findLargestElement (a, n, a11, a12, a21, a22);

	int count = 0;
	SVD_CELL_TYPE old_value = 0;
	/* FIXME This loop may not be synthesizable */

CONVERGENCE_LOOP:
	while (fp_abs (le.value) >
#ifndef REAL_FLOAT
			SVD_CELL_TYPE(SVD_PRECISION)
#else
			SVD_PRECISION
#endif
			&& fp_abs(le.value - old_value) > SVD_CELL_TYPE(MIN_MOVEMENT)
			) {
		old_value = le.value;
		count++;
		rotate (a, n, u, v, a11, a12, a21, a22);
		le = findLargestElement (a, n, a11, a12, a21, a22);
#ifndef REAL_FLOAT
		if (!(count % 10))
		cerr << "current iteration: " << count << "; " << le.value.to_double() << ": " <<
			old_value.to_double() << endl;
#endif
		
	}
#ifndef REAL_FLOAT
		cerr << "current iteration: " << count << "; " << le.value.to_double() << ": " <<
			old_value.to_double() << endl;
#endif

	cout << "loop count: " << count << endl;

	reorder (a, n, u, v);
	wait();

	// Copy over the singular values in a to s
COPY_SINGULAR_VALUES:
	for (i = 1; i < MAX_SIZE; i++) {
		if (i == n) break;
		SVD_CELL_TYPE tmp;
		tmp = s[(i * n) + i];
		wait();
		s[i] = tmp;
		wait();
		s[(i * n) + i] = 0;
		wait();
	}

	return;
}

void svd::rotate (SVD_CELL_TYPE *a, int dimension, SVD_CELL_TYPE *u, SVD_CELL_TYPE *v, int *x11, int *x12, int *x21, int *x22) {
	SVD_CELL_TYPE a11, a12, a21, a22; // elements of the sub-matrix
	SVD_CELL_TYPE alpha, beta; // angles used in rotations; alpha is angle of left rotation and beta is angle of right rotation
	SVD_CELL_TYPE cosA, sinA, cosB, sinB;
	SVD_CELL_TYPE X, Y; // temporary values used in calculating angles

	// Assign elements of sub-matrix to the their actual values from a
	a11 = a [x11[0] * dimension + x11[1]];
	wait();
	a12 = a [x12[0] * dimension + x12[1]];
	wait();
	a21 = a [x21[0] * dimension + x21[1]];
	wait();
	a22 = a [x22[0] * dimension + x22[1]];
	wait();

	// Calculate angles and sin and cos of those angles using the closed
	// formulas found.
#ifdef CTOS_SC_FIXED_POINT
	X = sld::atan2_cordic_func<CORDIC_ITER, WL, IWL>((SVD_CELL_TYPE) (a11+a22), (SVD_CELL_TYPE) (a21 - a12));
	Y = sld::atan2_cordic_func<CORDIC_ITER, WL, IWL>((SVD_CELL_TYPE) (a11-a22), (SVD_CELL_TYPE) (a21 + a12));
	//cout << "X, Y: " << X << "; " << Y << "; " << (X + Y) << "; " << (Y - X) << endl;

	alpha = (X + Y) * SVD_CELL_TYPE(0.5);//sld::div_func<WL, IWL, WL, IWL, WL, IWL>((X + Y), SVD_CELL_TYPE(2));
	beta = (Y - X) * SVD_CELL_TYPE(0.5);//sld::div_func<WL, IWL, WL, IWL, WL, IWL>((Y - X), SVD_CELL_TYPE(2));
	//cout << "a, b: " << alpha << "; " << beta << endl;

	sld::cos_sin_cordic_func<CORDIC_ITER, WL, IWL>(alpha, cosA, sinA);
	//cosA = sld::cos_cordic_func<CORDIC_ITER, WL, IWL>(alpha);
	//sinA = sld::sin_cordic_func<CORDIC_ITER, WL, IWL>(alpha);
	sld::cos_sin_cordic_func<CORDIC_ITER, WL, IWL>(beta, cosB, sinB);
	//cosB = sld::cos_cordic_func<CORDIC_ITER, WL, IWL>(beta);
	//sinB = sld::sin_cordic_func<CORDIC_ITER, WL, IWL>(beta);

	//cout << "rotate stats: " << cosA << "; " << sinA << "; " << cosB << "; " << sinB << endl;
#else
	X = atan((a21 - a12) / (a11 + a22));
	Y = atan((a21 + a12) / (a11 - a22));
	alpha = 0.5 * (X + Y);
	beta = 0.5 * (Y - X);

	cosA = cos (alpha); sinA = sin (alpha);
	cosB = cos (beta); sinB = sin (beta);
#endif

	// Create left rotation matrix, namely U_i which looks like this
	//
	//		| cosA	sinA |
	//		| -sinA	cosA |
	//
	identify (Ui, dimension);
	Ui [x11[0] * dimension + x11[1]] = Ui [x22[0] * dimension + x22[1]] = cosA;
	Ui [x12[0] * dimension + x12[1]] = sinA;
	Ui [x21[0] * dimension + x21[1]] = -sinA;
	// Rotate a
    // First on the left with Ui
    multiply (Ui, a, tempMatrix, dimension);
    copyMatrix (tempMatrix, a, dimension);
	// Apply rotation matrix Ui to U
    multiply (Ui, u, tempMatrix, dimension);
    copyMatrix (tempMatrix, u, dimension);
    // Create the right rotation matrix, namely V_i which looks like this
    // Note that I'm using Ui as Vi 
    //		| cosB	-sinB |
    //		| sinB	 cosB |
    //
    identify (Ui, dimension);
    Ui [x11[0] * dimension + x11[1]] = Ui [x22[0] * dimension + x22[1]] = cosB;
    Ui [x12[0] * dimension + x12[1]] = -sinB;
    Ui [x21[0] * dimension + x21[1]] = sinB;

    // Then on the right with Vi
    multiply (a, Ui, tempMatrix, dimension);
    copyMatrix (tempMatrix, a, dimension);
    multiply (v, Ui, tempMatrix, dimension);
    copyMatrix (tempMatrix, v, dimension);


    
/*

    // Create the right rotation matrix, namely V_i which looks like this
	//
	//		| cosB	-sinB |
	//		| sinB	 cosB |
	//
	identify (Vi, dimension);
	Vi [x11[0] * dimension + x11[1]] = Vi [x22[0] * dimension + x22[1]] = cosB;
	Vi [x12[0] * dimension + x12[1]] = -sinB;
	Vi [x21[0] * dimension + x21[1]] = sinB;

	// Rotate a
	// First on the left with Ui
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
*/
	return; 
}

LargestElement svd::findLargestElement (SVD_CELL_TYPE *matrix, int dimension, int *a11, int *a12, int *a21, int *a22) {
	LargestElement leTemp;
	int i, j;

	leTemp.value = 0;
	leTemp.rowNum = leTemp.colNum = -1;

	// Populate leArray such that the entry at index i contains information
	// about the largest element of row i
FLE_POPULATE_OUTER:
	for (i = 0; i < MAX_SIZE; i++) {
		if (i == dimension) break;

FLE_POPULATE_INNER:
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
		//leArray[i].value = leTemp.value;
		//leArray[i].rowNum = leTemp.rowNum;
		//leArray[i].colNum = leTemp.colNum;
		//leTemp.value = 0;
	}
	//leTemp.value = 0;
	//leTemp.rowNum = leTemp.colNum = -1;
	// Iterate over leArray and find the largest element in the matrix as a
	// whole
/*
FLE_ITERATE_LOOP:
	for (i = 0; i < MAX_SIZE; i++) {
		if (i == dimension) break;
		if (fp_abs (leArray [i].value) > fp_abs (leTemp.value)) {
			leTemp.value = leArray [i].value;
			leTemp.rowNum = leArray [i].rowNum;
			leTemp.colNum = leArray [i].colNum;
		}
	}
*/
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

void svd::transpose (SVD_CELL_TYPE *matrix, int dimension) {
	SVD_CELL_TYPE temp, temp2;
	int row, col;

TRANSPOSE_OUTER:
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;

TRANSPOSE_INNER:
		for (col = (row + 1); col < MAX_SIZE; col++) {
			/* TODO rework indexing */
			if (col == dimension) break;
			if (col == row) {continue;} // skip diagonal elements
			temp = matrix [(row * dimension) + col];
			wait();
			temp2 = matrix [(col * dimension) + row];
			wait();
			matrix [(row * dimension) + col] = temp2;
			wait();
			matrix [(col * dimension) + row] = temp;
			wait();
		}
	}
	return;
}

void svd::reorder (SVD_CELL_TYPE *a, int dimension, SVD_CELL_TYPE *u, SVD_CELL_TYPE *v) {
	int row, col, x, largestElementRow;
	SVD_CELL_TYPE temp; // stores the largest singular value
    //Replace p with Ui because why not?
    


REORDER_OUTER:
	for (x = 0; x < MAX_SIZE; x++) {
		if (x == dimension) break;
		temp = 0.0;
		identify (Ui, dimension);

REORDER_INNER:
		for (row = x; row < MAX_SIZE; row++) {
			if (row == dimension) break;
			col = row;
			if (fp_abs (a [row * dimension + col]) > fp_abs (temp)) {
				temp = a [row * dimension + col];
				largestElementRow = row;
			}
		}
		swapRows (Ui, x, largestElementRow, dimension);
		// Reorder a
		multiply (Ui, a, tempMatrix, dimension); copyMatrix (tempMatrix, a, dimension);
		multiply (a, Ui, tempMatrix, dimension); copyMatrix (tempMatrix, a, dimension);
		// Reorder u
		multiply (Ui, u, tempMatrix, dimension); copyMatrix (tempMatrix, u, dimension);
		// Reorder v
		multiply (v, Ui, tempMatrix, dimension); copyMatrix (tempMatrix, v, dimension);
	}

	transpose (u, dimension);
	transpose (v, dimension);
	identify (Ui, dimension);
REORDER_FIND_NEG:
	for (row = 0; row < MAX_SIZE; row++) {
		if (row == dimension) break;
		col = row;
		if (a [row * dimension + col] < 0)
			Ui[row * dimension + col] = -1.0;
	}
	multiply (Ui, a, tempMatrix, dimension);
	copyMatrix (tempMatrix, a, dimension);
	multiply (u, Ui, tempMatrix, dimension);
	copyMatrix (tempMatrix, u, dimension);
/* Old version 
REORDER_OUTER:
for (x = 0; x < MAX_SIZE; x++) {
    if (x == dimension) break;
    temp = 0.0;
    identify (p, dimension);

REORDER_INNER:
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
REORDER_FIND_NEG:
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
*/ 
    return;
}

// Swap row a with row b of matrix m
void svd::swapRows (SVD_CELL_TYPE *m, int a, int b, int dimension) {
	SVD_CELL_TYPE temp;

SWAP_ROW:
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

LOAD_OUTER:
		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
LOAD_INNER:
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
		input_done.write(true);
		do { wait(); } while (!process_start.read());
		input_done.write(false);
		do { wait(); } while (process_start.read());
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

OUTPUT_S_OUTER:
		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
OUTPUT_S_INNER:
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

OUTPUT_U_OUTER:
		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
OUTPUT_U_INNER:
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

OUTPUT_V_OUTER:
		for (int i = 0; i < MAX_SIZE; ++i) {
			if (i == rows) break;
OUTPUT_V_INNER:
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

		for (int i = 0; i < MAX_SIZE; ++i) {
			for (int j = 0; j < MAX_SIZE; ++j) {
				SVD_CELL_TYPE tmp;
				tmp = matrix_in[i * rows + j];
				wait();
				s[i * rows + j] = tmp;
				wait();
			}
		}

		jacobi(s, rows, s, u, v);

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
#ifdef __CTOS__
SC_MODULE_EXPORT(svd)
#endif
