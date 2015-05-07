#ifndef __SVDTB_H__
#define __SVDTB_H__

#define SC_INCLUDE_FX
#include "systemc.h"
#include <ctos_flex_channels.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <ostream>
#include "svd_data.h"

//#define VERBOSE
#define MAX_ERROR (0.0001)

SC_MODULE(svd_tb) {
	sc_in<bool> clk;
	sc_in<bool> rst;
	sc_out<bool> rst_dut;

	// DMA requests interface from memory to device
	sc_in<unsigned>   rd_index;     // array index (offset from starting address)
	sc_in<unsigned>   rd_length;    // burst size (in words)
	sc_in<bool>       rd_request;   // transaction request
	sc_out<bool>      rd_grant;     // transaction grant

	// DMA requests interface from device to memory
	sc_in<unsigned>   wr_index;   // array index (offset from starting address)
	sc_in<unsigned>   wr_length;  // burst size (in words)
	sc_in<bool>       wr_request; // transaction request
	sc_out<bool>      wr_grant;   // transaction grant

	// input data readen by load_input
	put_initiator<SVD_CELL_TYPE> bufdin;
	// output data written by store output
	get_initiator<SVD_CELL_TYPE> bufdout;

	sc_out<unsigned> conf_size;
	sc_out<bool>     conf_done;

	// computation complete. Written by store_output
	sc_in<bool> svd_done;

	void dmac(void);

	SC_CTOR(svd_tb) {
		SC_CTHREAD(dmac, clk.pos()); 
		reset_signal_is(rst, false);

		bufdin.clk_rst(clk,rst);
		bufdout.clk_rst(clk,rst);
	}

	int get_mismatches() {return mismatches;}

	private:
	void fill_buf(void);
	void compute_golden_model(void);
	int mismatches;
	int mat_size;

	SVD_CELL_TYPE input_matrix[SVD_INPUT_SIZE(MAX_SIZE)];
	SVD_CELL_TYPE output_matrix[SVD_OUTPUT_SIZE(MAX_SIZE)];

	double golden_input_matrix[SVD_INPUT_SIZE(MAX_SIZE)];
	double golden_matrix[SVD_OUTPUT_SIZE(MAX_SIZE)];
}; 
#endif 
