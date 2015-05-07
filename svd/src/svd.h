#ifndef _SVD_H_
#define _SVD_H_

#define SC_INCLUDE_FX
#include "systemc.h"
#include <ctos_flex_channels.h>
//#include <flex_channels.hpp>

#include "svd_data.h"

#define NUM_OUTPUT_MATRIX 3

#define SVD_PRECISION (0.000000000001)
#define MIN_MOVEMENT  (0.000000000001)
#define CORDIC_ITER 80

//#define VERBOSE

typedef struct {
	SVD_CELL_TYPE value;
	char rowNum;
	char colNum;
} LargestElement;

SC_MODULE(svd) {
	sc_in<bool> clk;
	sc_in<bool> rst;

	// DMA requests interface from memory to device
	sc_out<unsigned> rd_index;   // array index (offset from base address)
	sc_out<unsigned> rd_length;  // burst size
	sc_out<bool>     rd_request; // transaction request
	sc_in<bool>      rd_grant;   // transaction grant

	// DMA requests interface from device to memory
	sc_out<unsigned> wr_index;   // array index (offset from base address)
	sc_out<unsigned> wr_length;  // burst size
	sc_out<bool>     wr_request; // transaction request
	sc_in<bool>      wr_grant;   // transaction grant

	// input data read by load_input
	get_initiator<SVD_CELL_TYPE> bufdin;
	// output data written by store output
	put_initiator<SVD_CELL_TYPE> bufdout;

	sc_in<unsigned> conf_size;
	sc_in<bool>     conf_done;

	// computation complete
	sc_out<bool>    svd_done;

	void config_svd(void);
	void load_input(void);
	void store_output(void);
	void process_svd(void);

	SC_CTOR(svd) {
		SC_CTHREAD(config_svd, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(load_input, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(store_output, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(process_svd, clk.pos());
		reset_signal_is(rst, false);

		bufdin.clk_rst(clk,rst);
		bufdout.clk_rst(clk,rst);
	}

	private:
	//Written by config_debayer
	sc_signal<unsigned> size;
	sc_signal<bool>     init_done;

	//Written by load_input
	sc_signal<bool>     input_done;

	//Written by process_debayer
	sc_signal<bool>     process_start;
	sc_signal<bool>     process_done;

	//Written by store_output
	sc_signal<bool>     output_start;

	/* SVD functions */
	void multiply (SVD_CELL_TYPE *left, SVD_CELL_TYPE *right, SVD_CELL_TYPE *result, int dimension);
	void jacobi (SVD_CELL_TYPE *a, int n, SVD_CELL_TYPE *s, SVD_CELL_TYPE *u, SVD_CELL_TYPE *v);
	LargestElement findLargestElement (SVD_CELL_TYPE *matrix, int dimension, int *a11, int *a12, int *a21, int *a22);
	void copyMatrix (SVD_CELL_TYPE *a, SVD_CELL_TYPE *b, int dimension);
	void identify (SVD_CELL_TYPE *matrix, int dimension);
	void rotate (SVD_CELL_TYPE *a, int dimension, SVD_CELL_TYPE *u, SVD_CELL_TYPE *v, int *x11, int *x12, int *x21, int *x22);
	void transpose (SVD_CELL_TYPE *matrix, int dimension);
	void reorder (SVD_CELL_TYPE *a, int dimension, SVD_CELL_TYPE *u, SVD_CELL_TYPE *v);
	void swapRows (SVD_CELL_TYPE *m, int a, int b, int dimension);

	/* scratchpad matrices */
	SVD_CELL_TYPE Ui[MAX_SIZE * MAX_SIZE];
	//SVD_CELL_TYPE Vi[MAX_SIZE * MAX_SIZE];
	//SVD_CELL_TYPE tempResult[MAX_SIZE * MAX_SIZE];
	SVD_CELL_TYPE tempMatrix[MAX_SIZE * MAX_SIZE];
	//SVD_CELL_TYPE p[MAX_SIZE * MAX_SIZE];
	LargestElement leArray[MAX_SIZE];

	SVD_CELL_TYPE matrix_in[MAX_SIZE * MAX_SIZE];

	SVD_CELL_TYPE s[MAX_SIZE * MAX_SIZE];
	SVD_CELL_TYPE u[MAX_SIZE * MAX_SIZE];
	SVD_CELL_TYPE v[MAX_SIZE * MAX_SIZE];
};

#endif
