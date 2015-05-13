#ifndef BLARGEN_H
#define BLARGEN_H

#include "systemc.h"
#include <ctos_flex_channels.h>

SC_MODULE(blargen) {

	sc_in<bool> clk;
	sc_in<bool> rst;

	b_get_initiator<unsigned> data_in;
	b_put_initiator<unsigned> data_out;

	void beh(void);

	SC_CTOR(blargen) {
		SC_CTHREAD(beh, clk.pos());
		reset_signal_is(rst, false);

		data_in.clk_rst(clk, rst);
		data_out.clk_rst(clk, rst);
	}

	private:
	unsigned data[16];
};

#endif
