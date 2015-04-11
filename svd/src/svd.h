#ifndef _SVD_H_
#define _SVD_H_

#include "systemc.h"
//#include <ctos_flex_channels.h>
#include <flex_channels.hpp>

#include "svd_data.h"

#define NUM_OUTPUT_MATRIX 3

#define VERBOSE

SC_MODULE(svd) {
	sc_in<bool> clk;
	sc_in<bool> rst;

#if 0
	sc_in<bool>        data_in_req;
	sc_out<bool>       data_in_grant;
	sc_in<svd_token>   data_in;

	sc_out<bool>        data_out_req;
	sc_in<bool>         data_out_grant;
	sc_out<svd_token>   data_out;
#endif

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

	SVD_CELL_TYPE matrix_in[MAX_SIZE * MAX_SIZE];
	
	SVD_CELL_TYPE s[MAX_SIZE * MAX_SIZE];
	SVD_CELL_TYPE u[MAX_SIZE * MAX_SIZE];
	SVD_CELL_TYPE v[MAX_SIZE * MAX_SIZE];
};

#endif
