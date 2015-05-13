#ifndef BLARGEN_H
#define BLARGEN_H

#include "systemc.h"
#include <ctos_flex_channels.h>

#define SIZE (2*64*64)

SC_MODULE(blargen) {

	sc_in<bool> clk;
	sc_in<bool> rst;

	b_get_initiator<unsigned> data_in;
	b_put_initiator<unsigned> data_out;

	sc_out<bool> done;

	void input(void);
	void process(void);
	void output(void);

	SC_CTOR(blargen) {
		SC_CTHREAD(input, clk.pos());
		reset_signal_is(rst, false);

		SC_CTHREAD(process, clk.pos());
		reset_signal_is(rst, false);

		SC_CTHREAD(output, clk.pos());
		reset_signal_is(rst, false);

		data_in.clk_rst(clk, rst);
		data_out.clk_rst(clk, rst);
	}

	private:
	unsigned d_in[SIZE];
	unsigned d_out[SIZE];

	sc_signal<bool> done_input;
	sc_signal<bool> done_process;
};

#endif
