#ifndef _SVD_WRAPPER_H_
#define _SVD_WRAPPER_H_

#define SC_INCLUDE_FX
#include "systemc.h"
#include <ctos_flex_channels.h>

#include "blargen.h"

SC_MODULE(blargen_wrapper) {
	sc_in<bool> clk;
	sc_in<bool> rst;

	get_initiator<unsigned> data_in;
	put_initiator<unsigned> data_out;

	void handle_input(void);
	void handle_output(void);
	void handle_reset(void);

	SC_CTOR(blargen_wrapper):
		dut("svd")
 	{
		/* processes */
		SC_CTHREAD(handle_input, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(handle_output, clk.pos());
		reset_signal_is(rst, false);
		SC_CTHREAD(handle_reset, clk.pos());
		reset_signal_is(rst, false);

		/* external interface */
		data_in.clk_rst(clk, rst);
		data_out.clk_rst(clk, rst);

		/* this -> signals */
		data_to_dut(bufdin);
		data_from_dut(bufdout);
		data_to_dut.clk_rst(clk, rst);
		data_from_dut.clk_rst(clk, rst);

		/* signals -> dut */
		dut.clk(clk);
		dut.rst(dut_rst);
		dut.data_in(bufdin);
		dut.data_out(bufdout);
		dut.done(dut_done);
	}

private:
	blargen dut;
	put_get_channel<unsigned> bufdin;
	put_get_channel<unsigned> bufdout;

	put_initiator<unsigned> data_to_dut;
	get_initiator<unsigned> data_from_dut;

	sc_signal<bool> dut_done;
	sc_signal<bool> dut_rst;
};


#endif
