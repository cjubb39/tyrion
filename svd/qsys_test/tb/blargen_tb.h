#ifndef BLARGEN_TB_H
#define BLARGEN_TB_H

#include "systemc.h"
#include <ctos_flex_channels.h>

SC_MODULE(blargen_tb) {
	sc_in<bool> clk;
	sc_in<bool> rst;

	b_put_initiator<unsigned> data_to_dut;
	b_get_initiator<unsigned> data_from_dut;

	void beh(void);

	SC_CTOR(blargen_tb) {
		SC_CTHREAD(beh, clk.pos());
		reset_signal_is(rst, false);

		data_to_dut.clk_rst(clk,rst);
		data_from_dut.clk_rst(clk,rst);
	}
};

#endif
